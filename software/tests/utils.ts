/**
 * Checks if a Promise rejects. If so, RESOLVES with the error (or, if falsy or empty `{}`, with `true`), and otherwise resolves with false (no error thrown).
 * Never rejects.
 * @param f Promise to check.
 * @returns False if resolved, error (or true) if rejected.
 */
export const promiseToReject = (f: Promise<any>): Promise<boolean> => {
 return new Promise(res => {
   f.catch((err)=>{ res((err === {} || !err) ? true : err) }).then(()=>{ res(false) });
 });
};