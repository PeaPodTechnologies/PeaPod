# PeaPod Automation Subsystem

# Assembly

## Materials

### Tools

Required:
- Soldering Iron, Solder, and Flux
- Wire Strippers (20-30 AWG)
- 3D-Printer and PETG filament (or printing service)

Optional, but recommended:
- Helping Hands (for soldering)
- Multimeter (for testing connections)
- Oscilloscope (for testing signal integrity)

### Purchasing

See `./hardware/automation_bom_purchase.csv` for the purchase bill of materials. See `./hardware/gerber/` for PCB manufacturing files. See `./hardware/automation_bom_components.csv` for the electronic components bill of materials (DigiKey).

- 1x Automation Subsystem Motherboard PCB
- 1x Raspberry Pi Zero 2 W with 2x stacking headers, and:
  - 1x Micro-SD card with adapter
  - 1x Micro-USB power supply
  - 1x Mini-HDMI to HDMI adapter
  - 1x micro-USB USB A hub
  - 1x USB A keyboard
  - 1x Raspberry Pi Camera Module with 1x coupling board and cables
- 1x Adafruit ESP32 Feather V2 with 2x stacking headers and 1x USB C-to-A cable
- 1x Adafruit 4x20-character LCD screen with 1x Adafruit LCD backpack
- 1x Adafruit Stemma QT Rotary Encoder Breakout with 1x knob
- 1x Adafruit Stemma QT 3V to 5V Level Booster Breakout
- 3x Adafruit Stemma QT 5-Port Hub
- 1x Adafruit 7-Segment Display Featherwing
- 10x Adafruit Stemma QT JST SH 4-pin cables
- 8x Adafruit Ethernet Round Panel-Mount Extension Cables

### Fabrication

See `./hardware/fabrication/` for 3D-printed parts.

- 1x Automation Module Case (3D-printed)
- 1x Automation Module Case Lid (3D-printed)

## Instructions

<!-- TODO: Perma-proto -->

1. Solder stacking headers to the motherboard: one 2x20 at U1 for the Raspberry Pi Zero 2 W, and one each 1x16 and 1x12 at U2 for the Adafruit ESP32 Feather V2.
2. Mount the motherboard to the case using the included M4 screws and standoffs, ensuring the motherboard is oriented such that the USB ports on the silkscreen are facing the case opening.
3. Solder 2x20 stacking header pins to the Raspberry Pi Zero 2 W, ensuring the pins are oriented to connect with the motherboard headers (chipset up).
4. Solder 1x16 and 1x12 stacking header pins to the Adafruit ESP32 Feather V2 (if not pre-assembled), ensuring the pins are oriented to connect with the motherboard headers (chipset up).
5. Prepare the micro-SD card according to the instructions in `/software/README.md` (using the SD card adapter). Insert the micro-SD card into the Raspberry Pi Zero 2 W.
6. Insert the Raspberry Pi Zero 2 W and Adafruit ESP32 Feather V2 into their respective headers on the motherboard, ensuring proper orientation (match footprint to the silkscreen).
7. Solder the included header pins to the Adafruit 4x20-character LCD screen. Align the Adafruit LCD backpack with the LCD screen header pins as shown [here](https://cdn-learn.adafruit.com/assets/assets/000/001/882/original/lcds___displays_option1_t.jpeg) and solder the backpack to the header pins. Mount the screen to the case lid using the included M3 screws and nuts.
8. Mount the Adafruit Stemma QT Rotary Encoder Breakout to the case lid using the included M2.5 screws and nuts. Fasten the knob to the rotary encoder.
9. Connect one end of an Adafruit Stemma QT JST SH 4-pin cable to the Adafruit ESP32 Feather V2 I2C port. Connect the other end to the Adafruit Stemma QT 3V to 5V Level Booster Breakout, and then in series to the Adafruit LTC4311 I2C Extender/Active Terminator using another Stemma QT cable. Connect the active terminator to three Adafruit Stemma QT 5-Port Hubs in series, making a total of ten open active Stemma QT ports.
10. Cut and strip four Adafruit Stemma QT JST SH 4-pin cables, making eight one-ended connectors. Cut the male ends off of the eight Adafruit Ethernet Round Panel-Mount Extension Cables and strip the RJ45 wires. Solder the stripped wires to the one-ended connectors as follows:
    - Red (5V) to RJ45 orange pair
    - Black (GND) to RJ45 brown pair
    - Blue (SDA) to RJ45 blue pair
    - Yellow (SCL) to RJ45 green pair
11. Connect the eight one-ended connectors to eight of the open ports on the 5-Port Hubs. Connect the screen backpack to any open port on the 5-Port Hubs. Connect the Adafruit Stemma QT Rotary Encoder Breakout to any open port on the 5-Port Hubs.
12. Mount the Adafruit 7-Segment Display Featherwing directly to the Adafruit ESP32 Feather V2 stacking headers.
13. Flash the Adafruit ESP32 Feather V2 with the `/software/microcontroller/` firmware according to the instructions in `/software/README.md`.
14. Attach the small end of the orange-colored camera flex cable to the Raspberry Pi Zero 2 W camera port. Attach the large end to the coupling board. Attach one end of the long camera flex cable to the coupling board, and the other end to the camera module. Ensure that all cables are oriented such that the metal connectors on the cable are facing the PCB.
15. Connect a display to the mini-HDMI port on the Raspberry Pi Zero 2 W.
16. Connect the USB hub to the micro-USB port labelled "USB" on the Raspberry Pi Zero 2 W. Connect the keyboard to the USB hub.
17. Connect the power supply to the micro-USB port labelled "PWR" on the Raspberry Pi Zero 2 W.

# Testing

## Power-On Test

**Protocol**: Power on the system. Observe the behavior of the Raspberry Pi Zero 2 W via the connected display. Observe the behavior of the Adafruit ESP32 Feather V2 via the built-in LED (and 7-segment display and/or LCD display, if applicable).

**Validation**: The Raspberry Pi Zero 2 W boots successfully, as evidenced by the display output. The Adafruit ESP32 Feather V2 boots successfully, as evidenced by the LED behavior. 7-segment display and/or LCD display show expected output (if applicable): variable readout and "PeaPod Initializing...", respectively.

**Failure Protocol**: 

1. *Raspberry Pi Zero 2 W Fails to Boot*: Check power supply connection and ensure the micro-SD card is properly inserted. If issue persists, format and flash the micro-SD card according to the instructions in `/software/README.md` and repeat testing.
2. *Adafruit ESP32 Feather V2 Fails to Boot*: Check connections to the motherboard and ensure the board is properly seated in the header. If issue persists, reflash the Adafruit ESP32 Feather V2 using the appropriate firmware and repeat testing.

## Serial Communication Test

**Protocol**: Execute `yarn buildapi && node serialtest.mjs`. Select the appropriate serial port for the Adafruit ESP32 Feather V2 when prompted (i.e. `/dev/ttyS0`). Observe the output in the terminal.

**Validation**: The terminal outputs JSON-formatted sensor data at regular intervals (e.g. every second), as sent from the Adafruit ESP32 Feather V2.

**Failure Protocol**: 

1. *Serial Open Fails*: Check wiring connections to the motherboard and ensure the Adafruit ESP32 Feather V2 is properly seated in the header. Ensure no other applications are using the same serial port.
2. *No Data*: Check that the Adafruit ESP32 Feather V2 is running the appropriate firmware and is programmed to send serial data to the appropriate interface (i.e. Serial for USB vs. Serial1 for RX/TX UART). Check wiring connections to the motherboard and ensure the Adafruit ESP32 Feather V2 is properly seated in the header.

## Camera Test

**Protocol**: Execute `libcamera-hello -t 0`. Observe the output in the terminal and the connected display.

**Validation**: The terminal outputs a continuous stream of camera data, and the connected display shows the camera feed in real time.

**Failure Protocol**: Check camera flex cable connections to the Raspberry Pi Zero 2 W and coupling board, ensuring proper orientation. Ensure the camera module is properly connected to the coupling board. Check that the camera module is functional (e.g. by testing with another Raspberry Pi or camera board, if available). Ensure that the camera interface is enabled in the Raspberry Pi configuration settings.

## Firmware Flash Test

**Protocol**: Execute `pio run -t upload`. Observe the output in the terminal.

**Validation**: The terminal outputs messages indicating successful compilation and upload of the firmware to the Adafruit ESP32 Feather V2.

**Failure Protocol**:
1. *Compilation Fails*: Check that PlatformIO is properly installed and configured. Ensure that the correct board and environment are selected in the `platformio.ini` file. Review error messages for specific issues (e.g. missing libraries, syntax errors) and address accordingly.
2. *Upload Fails*: Check connections to the motherboard and ensure the Adafruit ESP32 Feather V2 is properly seated in the header. Ensure the correct serial port is selected for uploading. Check that the Adafruit ESP32 Feather V2 is in the appropriate mode for flashing (e.g. boot mode) and that any necessary drivers are installed.

## Dashboard Test

**Protocol**: Execute `yarn start`. Observe the output in the terminal and the connected display, and respond to prompts in the terminal as necessary to initiate the dashboard. Using a separate device on the same network, navigate to the appropriate local IP address and port number for the dashboard (e.g. `http://192.168.X.X:3000`). Observe the dashboard behavior and data readouts.

**Validation**: The terminal outputs messages indicating successful connection to the Adafruit ESP32 Feather V2 and receipt of sensor data. The separate device successfully loads the dashboard webpage, which displays real-time sensor data and updates appropriately.

**Failure Protocol**:
1. See *Serial Communication Test* failure protocols for issues related to serial communication between the Raspberry Pi Zero 2 W and Adafruit ESP32 Feather V2.
2. *Dashboard Fails to Load*: Ensure the separate device is on the same network as the Raspberry Pi Zero 2 W. Check that the correct local IP address and port number are being used to access the dashboard. Check for any firewall settings that may be blocking access to the port.
3. *Dashboard Loads but No Data*: Check the terminal output for any errors related to receiving or processing data from the Adafruit ESP32 Feather V2.

## Scheduler Test

**Protocol**: Execute `yarn start` and launch the dashboard. Add Event and Interval tasks using the floating action button with a plus icon. Observe the terminal output for scheduled task execution messages.

**Validation**: The terminal outputs messages indicating the execution of scheduled tasks at the appropriate times. States on the ESP32 Feather V2 change accordingly, as evidenced by the terminal output and dashboard readouts and hardware behaviour.

**Failure Protocol**:
1. *Tasks Not Executing*: Check that the scheduler code is properly implemented and that tasks are being added to the schedule correctly. Review terminal output for any errors related to task scheduling or execution.
2. *States Not Changing*: Check that the scheduled tasks are correctly sending commands to the Adafruit ESP32 Feather V2 and that the firmware on the Adafruit ESP32 Feather V2 is properly set up to receive and respond to these commands. Check wiring connections to the motherboard and ensure the Adafruit ESP32 Feather V2 is properly seated in the header.