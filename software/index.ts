import * as dns from 'dns';
import ora from 'ora';
import chalk from 'chalk';
import PeaPod from './lib/PeaPod';
const sleep = (millis : number) => {
    return new Promise(resolve => {
        setTimeout(resolve, millis);
    });
};

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

var loading : ora.Ora;

/**
 * Checks the internet connectivity.
 * @param {number} timeout Timeout in milliseconds.
 * @param {dns.Resolver} resolver DNS Resolver object.
 */
function checkInternet (timeout : number = 5000) : Promise<boolean> {
    const resolver = new dns.Resolver({timeout: timeout});
    return new Promise<boolean>(ret=>{
        resolver.resolve('www.google.com',err=> {
            if (err && err.code == "ENOTFOUND") {
                ret(false);
            } else {
                ret(true);
            }
        });
    });
}

async function main(){
    loading = ora({
        text: `Checking for ${chalk.blue('Internet')} connection...'`,
        spinner: defaultSpinner,
    }).start();

    if(!(await checkInternet(5))){
        throw new Error('Could not connect to the Internet.');
    }
    loading.succeed(`Connected to the ${chalk.blue('Internet')}!`);
    await sleep(1500);
    var peapod = new PeaPod();
    const user = await peapod.authenticate();
    await peapod.post()
    peapod.arduino?.start((msg)=>{
        console.log(`[${chalk.magenta(msg.type.toUpperCase())}] - ${JSON.stringify(msg.msg)}`);
    })
}

main();