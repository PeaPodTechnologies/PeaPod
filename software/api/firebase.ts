// Imports: Firebase
import {
  getDatabase, 
  ref, 
  push, 
  // set
} from 'firebase/database';
import {
  getStorage,
  ref as storageRef,
  uploadBytes,
} from 'firebase/storage';
import { DebugJsonMessage } from './types';
import { getAuth } from 'firebase/auth';
import { FirebaseError } from './errors';
import { readFileSync } from 'fs';

export const pushDebugMessage = (message: DebugJsonMessage, t?: string) => {
  const _r = ref(database, `messages/${t ?? 'default'}`);
  const r = push(_r, message);
  // set(r, message);
};

export const pushDebugMessages = (messages: DebugJsonMessage[], t?: string) => {
  if(messages.length === 0) return;
  const d = messages.reduce((l, msg) => {
    const label = msg.t ?? (t ?? 'default');
    if(!l[label]) l[label] = [];
    l[label].push(msg);
    return l;
  }, {});
  Object.keys(d).forEach(k => {
    const _r = ref(getDatabase(), `messageBatches/${k}`);
    const r = push(_r, d[k]);
    // set(r, d[k]);
  });
};

export type FileUploadTypes = 'application/pdf' | 'image/jpeg' | 'image/png';

export const uploadFile = (buf: ArrayBuffer | Uint8Array, name: string) => {
  const storage = getStorage();
  const auth = getAuth();

  if(!auth.currentUser) throw new FirebaseError('User not authenticated');
  
  const r = storageRef(storage, 'users/' + auth.currentUser.uid + '/' + (name ?? `upload-${Date.now()}`));

  return uploadBytes(r, buf);
};