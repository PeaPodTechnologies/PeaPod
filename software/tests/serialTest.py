#!/usr/bin/env python
# -*- coding: utf-8 -*-
import serial
test_string = "Testing Raspberry Pi GPIO serial port ...".encode('utf-8')
port = "/dev/ttyS0"
try:
  serialPort = serial.Serial(port, 9600, timeout = 2)
  print ("Serial port '", port, "' ready for test:", sep='')
  bytes_sent = serialPort.write(test_string)
  print ("Sent ", bytes_sent, " bytes: '", test_string, "'", sep='')
  loopback = serialPort.read(bytes_sent)
  if loopback == test_string:
    print ("Received ",len(loopback), " bytes: '", test_string, "'\nPort ", port," is setup correctly!\n", sep='')
  else:
    print ("Received incorrect data: '", loopback, "' on serial port '", port, "' loopback\n", sep='')
  serialPort.close()
except IOError:
  print ("Error on ", port,"\n")
