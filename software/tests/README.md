# Software Testing Suite

### Serial Port Test - `serialTest.py`

Test the funcionality of the Raspberry Pi Zero 2 W GPIO mini-UART serial port by tying the RX and TX pins together.

### Arduino Compilation & Upload Test - `blink.ino`

Test the functionality of the `arduino-cli` and `avrdude` utilities (and their custom configurations) by compilation the script and flashing the binary.

### Communications Protocol Test - `revision.ino`

Test communications between the Arduino and Raspberry Pi including timeout and revision checking. Essentially a skeleton of the main `PeaPodOS-Arduino.ino` script, with no sensors or actuators.