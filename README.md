# PeaPod

[![forthebadge](https://forthebadge.com/images/badges/made-with-crayons.svg)](https://forthebadge.com) [![forthebadge](https://forthebadge.com/images/badges/gluten-free.svg)](https://forthebadge.com) [![forthebadge](https://forthebadge.com/images/badges/open-source.svg)](https://forthebadge.com) [![forthebadge](https://forthebadge.com/images/badges/powered-by-coffee.svg)](https://forthebadge.com)

An isolated and automated growth environment with the goal of accurate environment control and precise realtime telemetry.

# Development

## Structure

Software for the host computer (i.e. RPi) is located within `./computer/`. Software for the target Arduino is located within `./computer/arduino/`, so that when the computer code is downloaded, the Arduino code can be deployed from the host computer.

## Requirements

Your computer can be used as the host computer (for the time being). Alternatively, a Raspberry Pi is used in the production setting.

1. Install _Node.JS_ and _npm_ (`sudo apt install nodejs npm` on Unix terminal);
2. Install _Node.JS_ dependencies (`npm i`);
3. ...

## Run

Execute `node .` to run the host computer software.

## Manual Target Arduino Software Deployment

TODO

## Todo

- [ ] Finish Readme