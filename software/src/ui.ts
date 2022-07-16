import ora from 'ora';
import * as blessed from 'blessed';
import { DataSet } from './publisher';

const DEFAULT_SPINNER: ora.Spinner = {
  interval: 50,
  frames: [
    "▁▁▁▁▁▁▁▁▁▁▁",
    "█▁▁▁▁▁▁▁▁▁▁",
    "██▁▁▁▁▁▁▁▁▁",
    "███▁▁▁▁▁▁▁▁",
    "████▁▁▁▁▁▁▁",
    "█████▁▁▁▁▁▁",
    "▁█████▁▁▁▁▁",
    "▁▁█████▁▁▁▁",
    "▁▁▁█████▁▁▁",
    "▁▁▁▁█████▁▁",
    "▁▁▁▁▁█████▁",
    "▁▁▁▁▁▁█████",
    "▁▁▁▁▁▁▁████",
    "▁▁▁▁▁▁▁▁███",
    "▁▁▁▁▁▁▁▁▁██",
    "▁▁▁▁▁▁▁▁▁▁█",
  ]
};

/**
 * Global spinner object.
 */
let spinner: ora.Ora = ora();

export namespace Spinner {
  /**
  * Start the loading spinner.
  * @param text Text to display.
  * @param spinner Spinner to use. Defaults to the default spinner.
  */
  export function start(text: string = "") {
    // If it's already spinning, just change the text
    if (spinner.isSpinning) {
      spinner.text = text;
    } else {
      // Otherwise, start a new one
      spinner = ora({ text, spinner: DEFAULT_SPINNER }).start();
    }
  };

  /**
  * Fail the loading spinner.
  * @param text Text to display.
  */
  export function fail(text: string = "") {
    // If it's spinning, change the text and fail
    if (spinner.isSpinning) {
      spinner.fail(text);
    } else {
      // Otherwise, start a new one and fail it
      spinner = ora({ text }).fail();
    }
  };

  /**
  * Succeed the loading spinner.
  * @param text Text to display.
  */
  export function succeed(text: string = "") {
    // If it's spinning, change the text and succeed
    if (spinner.isSpinning) {
      spinner.succeed(text);
    } else {
      // Otherwise, start a new one and succeed it
      spinner = ora({ text }).succeed();
    }
  };

  /**
   * Complete the loading spinner with info (blue `i`)
   * @param text Text to display.
   */
  export function info(text: string = "") {
    if (spinner.isSpinning) {
      // If it's spinning, change the text and set to info
      spinner.info(text);
    } else {
      // Otherwise, start a new one and info it
      spinner = ora({ text }).info();
    }
  };

  /**
   * If spinning: stop and clear the current spinner, log some text, then restart the spinner
   * Else: Just log
   * 
   * @param text Text to log
   */
  export function log(text: string) {
    let oldtext = spinner.text;
    if (spinner.isSpinning) {
      spinner.stop();
      console.log(text);
      spinner.start(oldtext);
    } else {
      console.log(text);
    }
  };
}

// Create a screen object.
let screen = blessed.screen({
  smartCSR: true
});

let box = blessed.box({
  top: 'top',
  left: 'left',
  width: '50%',
  height: '50%',
  border: {
    type: 'line'
  },
  style: {
    fg: 'white',
    bg: 'black',
    border: {
      fg: '#ffffff'
    }
  },
  align: 'left',
});

screen.append(box);

export namespace Screen {
  export function render() { box.focus(); screen.render(); }
  export function hide() { box.hide(); }
  export function setData(data: DataSet) { 
    let s = 'PeaPod\n';
    s += Object.entries(data).map((datapoint)=>{
      return datapoint[0].split('-').map(word=>
        word.slice(0, 1).toUpperCase() + word.slice(1).toLowerCase()
      ).join(' ') + ': ' + datapoint[1].toFixed(2);
    }).join('\n');
    box.setContent(s);
  }
}