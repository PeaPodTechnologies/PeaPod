# PeaPodOS <!-- omit in toc -->

[![issuesopen](https://img.shields.io/github/issues/PeaPodTechnologies/peapod)](https://github.com/PeaPodTech/PeaPod/issues) [![issuesclosed](https://img.shields.io/github/issues-closed/PeaPodTechnologies/peapod)](https://github.com/PeaPodTech/PeaPod/issues?q=is%3Aissue+is%3Aclosed) [![opensource](https://img.shields.io/badge/open-source-red)](https://github.com/PeaPodTechnologies/PeaPod/issues?q=is%3Aopen+is%3Aissue+label%3A%22Status%3A+Open%22) ![coffee](https://img.shields.io/badge/powered%20by-coffee-brown) [![24/7](https://img.shields.io/badge/Eat,%20Sleep,-PeaPod-darkgreen)](https://www.youtube.com/watch?v=2zWv9JC5G3w) [![FLDSMDFR](https://img.shields.io/badge/The-FLDSMDFR-orange)](https://www.youtube.com/watch?v=k8xFbWLUDoQ)

<!-- TODO: Build Statuses? -->
 
A cloud-connected isolated and automated plant growth environment, able to generate any environment from a combination of independent environment parameters. 

Designed as both a hassle-free food production system and a research tool for precise and distributed mapping of the plant-environment relationship.

***
## Table of Contents
- [Development](#development)
  - [Requirements](#requirements)
    - [Setting Up Raspberry Pi](#setting-up-raspberry-pi)
    - [Building from Repo](#building-from-repo)
    - [Running from Package](#running-from-package)
***

## Background

<!-- TODO: Control flow diagram, describe internal vs cloud software -->

***

# Development

## Requirements

> Note: In Development, any computer may be used. In Production, a Raspberry Pi Zero W 2 is used.

### Setting Up Raspberry Pi

1. Format your SD card with a single FAT partition.
2. Download the Raspberry Pi Imager [(Download)](https://www.raspberrypi.com/software/).
3. Flash the SD card with a *Raspberry Pi OS Lite* image.

> Note: In Future, a custom PeaPod image will be produced.

4. Plug in a keyboard and display, and power the device.
5. Login with the default username (`pi`) and password (`raspberry`).
6. Execute `sudo raspi-config` and perform these steps to setup the Pi:
   1. *System Options > Wireless Lan* - Setup WiFi
   2. *System Options > Hostname* - Set a unique host name (`e.g. john-peapod`)
   3. *System Options > Boot/Auto Login* - Select `Console Autologin`
   4. *Interface Options > Camera* - Enable Camera
   5. Locale
   6. Timezone
   7. Keyboard

> Note: In Future, this will be automated at startup.
7. Update Packages:
   1. `sudo apt-get update`
   2. `sudo apt-get install nodejs npm`
   3. `sudo npm i -g @peapodtech/peapodos --save`

### Building from Repo

1. Install Node.JS and `npm` [(Download)](https://nodejs.org/en/download/).
2. Clone the [PeaPod repository](https://github.com/PeaPodTechnologies/PeaPod).
3. Open a terminal and navigate to `/software` (this folder).
4. Run `npm run build` to download all JS dependencies and build the package.

Run `npm start` to launch the project.

### Running from Package

<!-- TODO: Publish package to npm -->

1. Install Node.JS and `npm` [(Download)](https://nodejs.org/en/download/).
2. Install this package with `npm i @peapodtech/peapodos`.

<!-- Uhhh other steps??? -->

Run `npm start` to launch the project.