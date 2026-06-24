# PeaPod Automation Subsystem

# Assembly

## Materials

### Purchased

See `./hardware/automation_bom_purchase.csv` for the purchase bill of materials. See `./hardware/gerber/` for PCB manufacturing files. See `./hardware/automation_bom_components.csv` for the electronic components bill of materials (DigiKey).

### Tools

Required:
- Soldering Iron, Solder, and Flux
- Wire Strippers (20-30 AWG)

Optional, but recommended:
- Helping Hands (for soldering)
- Multimeter (for testing connections)
- Oscilloscope (for testing signal integrity)

## Instructions

1. Solder stacking headers to the motherboard: one 2x20 at U1 for the Raspberry Pi Zero 2 W, and one each 1x16 and 1x12 at U2 for the Adafruit ESP32 Feather V2.
2. Solder 2x20 stacking header pins to the Raspberry Pi Zero 2 W, ensuring the pins are oriented to connect with the motherboard headers.
3. Solder 1x16 and 1x12 stacking header pins to the Adafruit ESP32 Feather V2 (if not pre-assembled), ensuring the pins are oriented to connect with the motherboard headers.
4. Prepare the micro-SD card according to the instructions in `/software/README.md` (using the SD card adapter). Insert the micro-SD card into the Raspberry Pi Zero 2 W.
5. Insert the Raspberry Pi Zero 2 W and Adafruit ESP32 Feather V2 into their respective headers on the motherboard, ensuring proper orientation (match footprint to the silkscreen).
6. Flash the Adafruit ESP32 Feather V2 with the `/software/microcontroller/` firmware according to the instructions in `/software/README.md`.
7. Assemble all subsystem modules in accordance with their instructions. Cut and strip four Qwiic wires, making eight one-ended connectors. Connect the modules to the Adafruit ESP32 Feather V2 I2C port using Adafruit Qwiic cables.
8. Optional: Mount the Adafruit 7-Segment Display Featherwing directly to the Adafruit ESP32 Feather V2 stacking headers.
9. Attach the small end of the orange-colored camera flex cable to the Raspberry Pi Zero 2 W camera port. Attach the large end to the coupling board. Attach one end of the long camera flex cable to the coupling board, and the other end to the camera module. Ensure that all cables are oriented such that the metal connectors on the cable are facing the PCB.
10. Connect a display to the mini-HDMI port on the Raspberry Pi Zero 2 W.
11. Connect the USB hub to the micro-USB port labelled "USB" on the Raspberry Pi Zero 2 W. Connect the keyboard to the USB hub.
12. Connect the power supply to the micro-USB port labelled "PWR" on the Raspberry Pi Zero 2 W.

# Testing

## Power-On Test

**Protocol**: Power on the system. Observe the behavior of the Raspberry Pi Zero 2 W via the connected display. Observe the behavior of the Adafruit ESP32 Feather V2 via the built-in LED (and 7-segment display and/or LCD display, if applicable).

**Validation**: The Raspberry Pi Zero 2 W boots successfully, as evidenced by the display output. The Adafruit ESP32 Feather V2 boots successfully, as evidenced by the LED behavior. 7-segment display and/or LCD display show expected output (if applicable): variable readout and "PeaPod Initializing...", respectively.

**Failure Protocol**: 

1. *Raspberry Pi Zero 2 W Fails to Boot*: Check power supply connection and ensure the micro-SD card is properly inserted. If issue persists, format and flash the micro-SD card according to the instructions in `/software/README.md` and repeat testing.
2. *Adafruit ESP32 Feather V2 Fails to Boot*: Check connections to the motherboard and ensure the board is properly seated in the header. If issue persists, reflash the Adafruit ESP32 Feather V2 using the appropriate firmware and repeat testing.

## Serial Communication Test

**Protocol**: Execute `yarn buildapi && node serialtest.mjs`. Select the appropriate serial port for the Adafruit ESP32 Feather V2 when prompted. Observe the output in the terminal.

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
3. *Dashboard Loads but No Data*: Check the terminal output for any errors related to receiving or processing data from the Adafruit ESP32 Feather V2. Ensure that the dashboard code is correctly set up to receive and display data.

## Scheduler Test

**Protocol**: Execute `yarn start` and launch the dashboard. Add Event and Interval tasks using the floating action button with a plus icon. Observe the terminal output for scheduled task execution messages.

**Validation**: The terminal outputs messages indicating the execution of scheduled tasks at the appropriate times. States on the ESP32 Feather V2 change accordingly, as evidenced by the terminal output and dashboard readouts and hardware behaviour.

**Failure Protocol**:
1. *Tasks Not Executing*: Check that the scheduler code is properly implemented and that tasks are being added to the schedule correctly. Review terminal output for any errors related to task scheduling or execution.
2. *States Not Changing*: Check that the scheduled tasks are correctly sending commands to the Adafruit ESP32 Feather V2 and that the firmware on the Adafruit ESP32 Feather V2 is properly set up to receive and respond to these commands. Check wiring connections to the motherboard and ensure the Adafruit ESP32 Feather V2 is properly seated in the header.