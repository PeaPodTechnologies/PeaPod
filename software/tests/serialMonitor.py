#!/usr/bin/env python
# -*- coding: utf-8 -*-
import serial
port = "/dev/ttyS0"
try:
  serialPort = serial.Serial(port, 115200)
  print ("Serial port '", port, "' open:", sep='')
  while True:
    if serialPort.in_waiting > 0:
      print(serialPort.read(serialPort.in_waiting).decode('unicode_escape'), end='')
except IOError:
  print ("Error on ", port,"\n")
  serialPort.close()