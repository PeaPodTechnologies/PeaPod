import ora from 'ora';

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
var Spinner: ora.Ora = ora();
export default Spinner;

/**
 * Start the loading spinner.
 * @param text Text to display.
 * @param spinner Spinner to use. Defaults to the default spinner.
 */
export const startSpinner = (text: string = "", spinner: ora.Spinner = defaultSpinner) => {
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
export const failSpinner = (text: string = "") => {
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
 export const succeedSpinner = (text: string = "") => {
    // If it's spinning, change the text and succeed
    if(Spinner.isSpinning){
        Spinner.succeed(text);
    } else {
        // Otherwise, start a new one and succeed it
        Spinner = ora({text}).succeed();
    }
}