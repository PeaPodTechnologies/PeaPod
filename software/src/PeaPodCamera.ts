import child from 'child_process';
import { Readable } from 'node:stream'

export type IPeaPodCamera = {
  stream(options?: PeaPodCameraOptions): Readable;
}

export type PeaPodCameraOptions = {
  height: number;
  width: number;
}

export default class PeaPodCamera {
  stream(options: PeaPodCameraOptions = {height: 480, width: 852}){
    return child.spawn('raspivid', [
      '--nopreview',
      '--width',
      `${options.width}`,
      '--height',
      `${options.height}`,
      '-o',
      '-'
    ], {
      stdio: ['ignore', 'pipe', 'inherit']
    }).stdout;
  }
}