import ora from 'ora';

namespace Spinner {
  const defaultSpinner : ora.Spinner = {
    interval: 50,
    frames: [
      "▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁",
      "█▁▁▁▁▁▁▁▁▁▁▁▁▁▁",
      "██▁▁▁▁▁▁▁▁▁▁▁▁▁",
      "███▁▁▁▁▁▁▁▁▁▁▁▁",
      "████▁▁▁▁▁▁▁▁▁▁▁",
      "█████▁▁▁▁▁▁▁▁▁▁",
      "▁█████▁▁▁▁▁▁▁▁▁",
      "▁▁█████▁▁▁▁▁▁▁▁",
      "▁▁▁█████▁▁▁▁▁▁▁",
      "▁▁▁▁█████▁▁▁▁▁▁",
      "▁▁▁▁▁█████▁▁▁▁▁",
      "▁▁▁▁▁▁█████▁▁▁▁",
      "▁▁▁▁▁▁▁█████▁▁▁",
      "▁▁▁▁▁▁▁▁█████▁▁",
      "▁▁▁▁▁▁▁▁▁█████▁",
      "▁▁▁▁▁▁▁▁▁▁█████",
      "▁▁▁▁▁▁▁▁▁▁▁████",
      "▁▁▁▁▁▁▁▁▁▁▁▁███",
      "▁▁▁▁▁▁▁▁▁▁▁▁▁██",
      "▁▁▁▁▁▁▁▁▁▁▁▁▁▁█",
    ]
  }
  
  // Global spinner
  export var Spinner: ora.Ora = ora();
  
  /**
  * Start the loading spinner.
  * @param text Text to display.
  * @param spinner Spinner to use. Defaults to the default spinner.
  */
  export const start = (text: string = "", spinner: ora.Spinner = defaultSpinner) => {
    // If it's already spinning, just change the text
    if(Spinner.isSpinning){
      Spinner.text = text;
      Spinner.spinner = spinner;
    } else {
      // Otherwise, start a new one
      Spinner = ora({text, spinner}).start();
    }
  }
  
  /**
  * Fail the loading spinner.
  * @param text Text to display.
  */
  export const fail = (text: string = "") => {
    // If it's spinning, change the text and fail
    if(Spinner.isSpinning){
      Spinner.fail(text);
    } else {
      // Otherwise, start a new one and fail it
      Spinner = ora({text}).fail();
    }
  }
  
  /**
  * Succeed the loading spinner.
  * @param text Text to display.
  */
  export const succeed = (text: string = "") => {
    // If it's spinning, change the text and succeed
    if(Spinner.isSpinning){
      Spinner.succeed(text);
    } else {
      // Otherwise, start a new one and succeed it
      Spinner = ora({text}).succeed();
    }
  }

  /**
   * Set the loading spinner to info (blue `i`)
   * @param text Text to display.
   */
  export const info = (text: string = "") => {
    if(Spinner.isSpinning) {
      // If it's spinning, change the text and set to info
      Spinner.info(text);
    } else {
            // Otherwise, start a new one and info it
      Spinner = ora({text}).info();
    }
  }
}

export default Spinner;