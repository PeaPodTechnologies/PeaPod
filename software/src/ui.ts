import ora from 'ora';

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
export let Spinner: ora.Ora = ora();

/**
* Start the loading spinner.
* @param text Text to display.
* @param spinner Spinner to use. Defaults to the default spinner.
*/
export function start(text: string = "", spinner: ora.Spinner = DEFAULT_SPINNER) {
  // If it's already spinning, just change the text
  if (Spinner.isSpinning) {
    Spinner.text = text;
    Spinner.spinner = spinner;
  } else {
    // Otherwise, start a new one
    Spinner = ora({ text, spinner }).start();
  }
};

/**
* Fail the loading spinner.
* @param text Text to display.
*/
export function fail(text: string = "") {
  // If it's spinning, change the text and fail
  if (Spinner.isSpinning) {
    Spinner.fail(text);
  } else {
    // Otherwise, start a new one and fail it
    Spinner = ora({ text }).fail();
  }
};

/**
* Succeed the loading spinner.
* @param text Text to display.
*/
export function succeed(text: string = "") {
  // If it's spinning, change the text and succeed
  if (Spinner.isSpinning) {
    Spinner.succeed(text);
  } else {
    // Otherwise, start a new one and succeed it
    Spinner = ora({ text }).succeed();
  }
};

/**
 * Complete the loading spinner with info (blue `i`)
 * @param text Text to display.
 */
export function info(text: string = "") {
  if (Spinner.isSpinning) {
    // If it's spinning, change the text and set to info
    Spinner.info(text);
  } else {
    // Otherwise, start a new one and info it
    Spinner = ora({ text }).info();
  }
};

/**
 * If spinning: stop and clear the current spinner, log some text, then restart the spinner
 * Else: Just log
 * 
 * @param text Text to log
 */
export function log(text: string) {
  let oldtext = Spinner.text;
  if (Spinner.isSpinning) {
    Spinner.stop();
    console.log(text);
    Spinner.start(oldtext);
  } else {
    console.log(text);
  }
};