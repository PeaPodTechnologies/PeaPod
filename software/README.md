# PeaPodOS <!-- omit in toc -->

[![issuesopen](https://img.shields.io/github/issues/PeaPodTechnologies/peapod)](https://github.com/PeaPodTech/PeaPod/issues) [![issuesclosed](https://img.shields.io/github/issues-closed/PeaPodTechnologies/peapod)](https://github.com/PeaPodTech/PeaPod/issues?q=is%3Aissue+is%3Aclosed) [![opensource](https://img.shields.io/badge/open-source-red)](https://github.com/PeaPodTechnologies/PeaPod/issues?q=is%3Aopen+is%3Aissue+label%3A%22Status%3A+Open%22) ![coffee](https://img.shields.io/badge/powered%20by-coffee-brown) [![24/7](https://img.shields.io/badge/Eat,%20Sleep,-PeaPod-darkgreen)](https://www.youtube.com/watch?v=2zWv9JC5G3w) [![FLDSMDFR](https://img.shields.io/badge/The-FLDSMDFR-orange)](https://www.youtube.com/watch?v=k8xFbWLUDoQ)

<!-- TODO: Build Statuses? -->
 
A cloud-connected isolated and automated plant growth environment, able to generate any environment from a combination of independent environment parameters. 

Designed as both a hassle-free food production system and a research tool for precise and distributed mapping of the plant-environment relationship.

***
### Table of Contents
- [Background](#background)
- [Development](#development)
    - [Setting Up Raspberry Pi](#setting-up-raspberry-pi)
***

# Background

<a href="https://www.youtube.com/watch?v=2zWv9JC5G3w" target="_blank"><img src="assets/control_flow.png" width=60% style="border: 5px solid #333"/></a>

***

# Development

<!-- TODO: Dev environment, testing modes, making an ENV file -->

### Setting Up Raspberry Pi

The following are performed on a computer:

1. Format a microSD card (>=4GB) with a single FAT partition.
2. Download the Raspberry Pi Imager [(Download)](https://www.raspberrypi.com/software/).
3. Flash the SD card with a *Raspberry Pi OS Lite* image.

> Note: In Future, a custom PeaPod image will be released with steps 4-6 already complete.

4. Plug in a keyboard and display, insert the microSD card, and power the Raspberry Pi device.

The following are performed on the Raspberry Pi:

5. Login with the default username (`pi`) and password (`raspberry`).
6. Execute `sudo raspi-config` and perform these steps to setup the Pi:
   1. *System Options > Wireless Lan* - Setup WiFi
   2. *System Options > Hostname* - Set a unique host name (`e.g. john-peapod`)
   3. *System Options > Boot/Auto Login* - Select `Console Autologin`
   4. Disable UART console
   5. *Interface Options > Camera* - Enable Camera

7. Set up localization:
   1. Locale
   2. Timezone
   3. Keyboard

> Note: In Future, step 8 will be automated at startup.
8. Update Packages:
   1. `sudo apt-get update`
   2. `sudo apt-get install nodejs npm`
   3. `sudo npm i -g @peapodtech/peapodos --save`

9. Import `.env` file with Firebase and auth keys (TBA), as well as the field `SERIALPORT="/dev/ttyS0"`. This is the GPIO mini-UART on the Raspberry Pi Zero 2 W.

11. Run the program by executing `peapodos`.