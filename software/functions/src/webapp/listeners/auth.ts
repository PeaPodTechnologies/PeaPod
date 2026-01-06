import * as functions from 'firebase-functions';
import {firestore} from 'firebase-admin';

export const newUser = functions.auth.user().onCreate((user)=>{
  return firestore().doc(`users/${user.uid}`).create({
    devicequota: 1,
    devicecount: 0,
  });
});

/**
 * On user account deletion, deletes user doc and removes access from projects and runs
 */
export const deleteUser = functions.auth.user().onDelete((user)=>{
  return [
    firestore().doc(`users/${user.uid}`).delete(),
    firestore().collection('projects').where('owners', 'array-contains', user.uid).get().then((docs)=>{
      docs.docs.forEach((doc)=>{
        firestore().collection(`projects/${doc.id}/runs`).where('owner', '==', user.uid).get().then((rundocs)=>{
          rundocs.docs.forEach((rundoc)=>{
            rundoc.ref.update({
              owner: undefined,
            });
          });
        });
        doc.ref.update({
          owners: firestore.FieldValue.arrayRemove(user.uid),
        });
      });
    }),
  ];
});
