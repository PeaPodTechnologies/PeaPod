import * as functions from 'firebase-functions';
import {firestore} from 'firebase-admin';

type GetDatasetsParams = {
  project: string,
  run: string
}

export const getDatasets = functions.https.onCall(async (data: GetDatasetsParams, context)=>{
  if (!context.auth || !context.auth.uid) {
    throw new functions.https.HttpsError('unauthenticated', 'Not authenticated.');
  }

  const rundoc = await firestore().doc(`projects/${data.project}/runs/${data.run}`).get();

  if (rundoc.get('owner') != context.auth.uid) {
    throw new functions.https.HttpsError('permission-denied', 'Permission denied');
  }

  return (await rundoc.ref.listCollections()).map((set)=>set.id);
});
