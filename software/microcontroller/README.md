# PeaPod Microcontroller Firmware

Makes use of I2CIP, FSM, and DebugJson libraries for QOS-2 I2C device state management and abstraction, program state and control flow, and bidirectional JSON-formatted newline-delimited serial communication, respectively. Supports a main menu via 16x2 character LCD and rotary encoder with push button, and a seven segment display.

# Architecture

## Main

**Globals**:
- Menu State (menu index: enum, submenu index: unsigned char, selected module index: unsigned char)
- FSM:
  - Chronograph
- I2CIP:
  - modules\[8\]
  - errlev\[8\]

**Setup**:

1. Initialize Serial communication (115200 baud).
2. Wait for Serial connection.
3. Instantiate PeaPod modules (Air, Watering, Lighting; See Module Callbacks).
4. Instantiate non-module devices:
   1. Seven-Segment Display (HT16K33)
   2. 16x2 Character LCD (JHD1313)
   3. Rotary Encoder with Push Button (Adafruit Seesaw)
5. Attach LCD print callback to menu state changes.
6. Print firmware `revision` to Serial.
7. Register callbacks:
   1. Cycle Callback (triggered on `Cycle` change):
      1. Set Chronograph to current time.
      2. Calculate and set `FPS`.
      3. Read Serial input (DebugJson) and route commands/config.
   2. Heartbeat Callback (Chronograph interval):
      1. Print `heartbeat` message to Serial.
      2. Print firmware `revision` to Serial.
      3. Print `FPS` to Serial.
      4. Print current `Cycle` to Serial.
   3. Module Callbacks (Chronograph interval):
      1. If MUX pings and module was not present, instantiate module. Add seven segment display where applicable. Call module-check call operator and set module error level.
      2. If MUX doesn't ping, set module error level.
      3. If error level indicates hardware has been lost, de-instantiate module.

**Loop**:

1. Increment `Cycle` (See Cycle Callback).
2. Read encoder input and update menu state.

**Subroutines**:

- onEncoderChange
- writeLCDMenu
- onEncoderPress

## Modules

### Base Module

> An abstract base class for PeaPod modules.

Defines one namespaced abstract Module class: `PeaPodModule`, that:
- has two private Hash Tables: one for storing registered Variables, and one for storing registered Flags,
- has a public method `registerVariable` for registering Variables to the module,
- has a public method `registerFlag` for registering Flags to the module,
- implements `deviceGroupFactory`, linking all device driver classes to the module,
- implements `handleCommand`, routing JSON `GASB` commands to devices by FQA,
- implements `handleConfig`, routing JSON key-value pairs to flags/variables by name,

### Air Module

> Monitors and controls air quality parameters such as temperature, humidity, and CO2 levels.

Defines three namespaced global Variables: `air_temperature`, `air_humidity`, and `air_co2`.

Defines three namespaced global Callbacks: `callback_temperature`, `callback_humidity`, and `callback_co2` that print the respective variable values to Serial when they are updated.

Defines one namespaced Module class: `PeaPodModuleAir`, that extends `PeaPodModule`, that:
- has two Chronograph intervals for reading sensors SHT45 and K30,
- defines two static callback functions for processing mean sensor readings from SHT45 and K30,
- on instantiation:
  - sets up the Chronograph intervals to read sensors at defined intervals and set the Variables,
  - sets up the Variable Callbacks to print updated values to Serial,
  - registers the Variables to the module.
- on destruction:
  - removes the Chronograph intervals from the Chronograph scheduler.

### Watering Module

> Controls the watering system.

Defines one namespaced global Flag: `enable_watering`.

Defines one namespaced Module class: `PeaPodModuleWatering`, that extends `PeaPodModule`, that:
- has two Chronograph intervals for setting the `enable_watering` flag to true and false, respectively,
- on instantiation:
  - sets up the Chronograph intervals to set/reset the `enable_watering` flag at defined intervals and phases,
  - sets up a Flag Callback to write on/off to a MCP23017 GPIO pin when the `enable_watering` flag is updated,
  - sets the `enable_watering` flag to false, setting the GPIO pin to off, and disabling the watering system,
  - registers the `enable_watering` Flag to the module.
- on destruction:
  - sets the `enable_watering` flag to false, setting the GPIO pin to off, and disabling the watering system,
  - removes the Chronograph intervals from the Chronograph scheduler.

### Lighting Module

> Controls the lighting system.

Defines three namespaced global Variables: `adc_voltage`, `pwm_lighting_red`, and `pwm_lighting_blue`.

Defines one namespaced global Flag: `enable_lighting`.

Defines two namespaced global Callbacks: `callback_adc_read` and `callback_adc_voltage` that read the ADC value and convert it to voltage, respectively.

Defines one namespaced Module class: `PeaPodModuleLighting`, that extends `PeaPodModule`, that:
- has three Chronograph intervals: one for reading the ADC, and two for setting the `enable_lighting` flag to true and false, respectively,
- defines two static callback functions: one for modulating the PCA9685 output based on PWM values and `enable_lighting` flag, and one for handling the `enable_lighting` flag updates (re-calling the PWM modulation callbacks),
- on instantiation:
  - sets up the Chronograph intervals to set/reset the `enable_lighting` flag at defined intervals and phases,
  - sets an initial value for the PWM channels,
  - sets up a Chronograph interval to read the ADC and set the `adc_voltage` variable,
  - sets up the ADC Variable Callbacks to read and convert ADC values to voltage and print to Serial,
  - sets up a Flag Callback to gate the PWM modulation based on the `enable_lighting` flag,
  - sets up two Variable Callbacks to modulate the PCA9685 outputs based on PWM values,
  - sets the `enable_lighting` flag to false, setting the PWM outputs off, and disabling the lighting system,
  - registers the Variables to the module,
  - registers the `enable_lighting` Flag to the module.
- on destruction:
  - sets the `enable_lighting` flag to false, setting the PWM outputs off, and disabling the lighting system,
  - removes the Chronograph intervals from the Chronograph scheduler.