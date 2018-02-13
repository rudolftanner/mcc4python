#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "192.168.1.103"
PORT = 4223
UID = "Aci" # Change XYZ to the UID of your Dual Relay Bricklet

import time

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_dual_relay import BrickletDualRelay

if __name__ == "__main__":
    	ipcon = IPConnection() # Create IP connection
    	dr = BrickletDualRelay(UID, ipcon) # Create device object

    	ipcon.connect(HOST, PORT) # Connect to brickd
    	# Don't use device before ipcon is connected

	print dr.get_state()
        time.sleep(0.5)
        dr.set_state(False, False)
	time.sleep(0.5)
	print dr.get_state()
   	ipcon.disconnect()
