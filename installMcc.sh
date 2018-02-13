#!/bin/bash

echo "Start..."

if [ "$1" = 1 ];
then	
	mkdir mcc
	cd mcc

	echo ">>install libusb"
	sudo apt-get install libusb-1.0-0 libusb-1.0-0-dev

	echo ">>install devtools"
	sudo apt-get install libudev-dev libfox-1.6-dev autotools-dev autoconf automake libtool

	echo ">>git clone lnx drivers"
	sudo git clone https://github.com/wjasper/Linux_Drivers
	echo ">>do rules"
	cd Linux_Drivers
	sudo cp 61-mcc.rules /etc/udev/rules.d/99-mcc.rules

	echo ">>git clone hidapi"
	cd ..
	sudo git clone https://github.com/signal11/hidapi.git

	sudo chown -hR pi *

	cd hidapi
	sudo ./bootstrap
	sudo ./configure
	sudo make
	sudo make install

	echo "reboot now..."
	#sudo reboot
elif [ "$1" = 2 ];
then
	echo ">>mcc libusb"
	cd /home/mcc/Linux_Drivers/USB/mcc-libusb
	sudo make clean
	sudo make
	sudo make install
	sudo ldconfig
	
	echo ">>run test for 1208fs"
	./test-usb1208fs
fi

echo "...END."
