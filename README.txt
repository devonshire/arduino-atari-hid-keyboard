Interface Atari ST Keyboard to USB HID Keyboard
===============================================

This code runs on an Arduino Leonardo to provide an interface between 
an Atari ST keyboard (520 STFM keyboard tested, others *may* work) and 
a USB HID keyboard device.

The purpose of this is to allow a small form factor PC (Raspberry Pi in
my case) to be installed in an Atari ST case and to use the original 
ST keyboard.

Developed for use with an Arduino Leonardo as it is able to act directly 
as a USB keyboard controller so doesn't require the Arduino firmware to 
be modified as some of the other Arduinos (eg. Uno) would do.

The Atari keyboard connector is wired to the Arduino as follows:

Atari 8-pin Keyboard Connector     Arduino
------------------------------     -------
1 0V                               GND
2 (Blanked off)
3 Not used
4 5V                               5V
5 RX                               RX1 (0)
6 TX                               TX1 (1)
7 Reset                            4 (can be changed in code)
8 Not used

Copyright Kevin Peat 2017
kevin 'at' kevinpeat.com
