// Imports: Firebase
import {
  getDatabase, 
  ref, 
  push, 
  // set
} from 'firebase/database';
import { DebugJsonMessage } from './types';

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