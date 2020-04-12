#!/usr/bin/env python3
import serial
import time
ser = serial.Serial('/dev/tty.wchusbserialfd110', 9600, timeout=0, bytesize=serial.EIGHTBITS)

count=1

while True:
  for line in ser.read():
    if chr(line) != '':
      print(str(count) + str(': ') + chr(line) )
      count = count+1
      time.sleep(100)
      ser.write(1)
