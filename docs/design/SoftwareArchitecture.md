# PeaPod Software Architecture

## Overview

### Best Practices

1. Computer appends timestamp;
2. Serial: 
   1. Read before sending;
   2. High baud rate;
   3. Categorized JSON messages;
<!-- 3. Sensor-Actuator relationship: actuator change function call ***inside*** sensor read function. -->

### Microcontroller Message Codes
 1. Sensor readings - `data`
 2. Errors - `error`
 3. Debug/Testing - `debug`
 4. Instruction responses - `info`
 5. All other debug

### Dataset Standardization

 >So that the microcontroller, computer, and cloud can all communicate effectively, minimize `label` translation. **Lowercase** and **hyphenated** (not underscored) for storage and transfer. Standard case and spaced for printing.

 Timestamps (milliseconds UTC): `timestamp`

 Data and Instruction Labels:

 - Air Temperature (°C): `air-temperature`
 - Relative Humidity (%RH): `air-humidity`
 - LED series activation (by shorthand name, %): `led-$NAME$` (i.e. `led-cool`)
 - So on...

# Microcontroller

> Arduino Micro. Acts as a low-level `interface` for the main computer.

## POST

Test Serial protocol: send software revision byte, recieve check byte or stop byte

**IF STOP BYTE**: null loop, wait to be programmed, and reboot

Test sensor protocols - per-sensor tests

Tell computer 'ready', wait to receive `program`.

## Setup

Receive initial `program`, set actuator settings.

## Main Cycle

### Collect Data

> For every sensor, if the sensor is ready, read from the sensor.

List of all `sensor` objects - abstract superclass for sensor library wrappers/inputs:
 - `read` - function *wrapper* (takes void* args, calls *actual* read, resets lastread to Millis(), returns float)
 - hardware id (string `id`)
 - sensor readable name (string `name`)
 - environment variable readable name (string `evname`)
 - sensor read time delta in ms (unsigned int `delta`)
 - last read timestamp (unsigned long `lastread`)
> Note: Millis() is long, overflows after 2^32-1 (~49.7 days), so if the current time is **less** than the previous time, something has gone wrong. Skip read, reset to Millis().

Store the latest copy of sesnor data locally as a list of (environment variable name - value) pairs.

Send the latest read individual sensor data to the Computer as JSON over serial.
> Let the main controller handle timestamping. Makes more sense, since that's where data is ***really*** being managed, and where the big decisions are made surrounding sed data. Also, microcontroller RTC sucks ass.

### Plan, Act

> Compare the sensor data to the required current state.

List of `actuator` objects - abstract superclass for actuator library wrappers/outputs:
- `set` function - sets actuator target, performs any necessary calculations (i.e. PID) (takes float arg, returns void)
- hardware id (string `id`)
- actuator readable name (string `name`)
- environment variable *dataset* name (string `evname`)

Actuate each actuator.

# Computer

> Raspberry Pi running a limited-input Node.JS CLI app acting as a pseudo-web-client (for IAM purposes), and as an IoT device (for PubSub purposes).

## POST and Update

Check wifi, bluetooth, camera, screen, etc hardware interfaces

Check internet, cloud service connectivity

IF NO INTERNET: Inform user, launch in `offline mode`

ELSE: 

- `Online mode`
- Check for latest computer software package revision
- IF COMPUTER UPDATE: 
  - offer user to update (inform reboot) => download latest software, install, reboot
- Check for latest microcontroller software revision, wait for microcontroller revision byte (acts as serial test)
- IF MICROCONTROLLER UPDATE: 
  - offer user to update (inform reboot) => send stop byte, download and install
- ELSE: Send check byte

## Setup

`Online mode`: 

- User login to cloud service
- Fetch projects, select project
- Fetch programs, select program
- Publish new run

`Offline mode`: 

- Fetch projects from filesystem, select project
- Select program
- Generate new run

Save start and end timestamps

## Main Cycle

Recieve sensor data from Microcontroller → Database

Peapod run: Notes, start date, end date, hardware id, **collection** of sensor documents

> Cloud Functions: metadata upon project/run creation, Realtime and Firestore sync