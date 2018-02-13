/*
 *
 *  Copyright (c) 2015 Warren J. Jasper <wjasper@ncsu.edu>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdint.h>
#include <time.h>

#include "pmd.h"
#include "usb-1208FS.h"

int fetchTemperature( int *packetSize, float *T )
{
  int flag;
  signed short svalue;
  uint8_t channel, gain;
  int i;
  float aVal[8];
  float calGain[8]  = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  float dcOffset[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  libusb_device_handle *udev = NULL;
  int ret;

  ret = libusb_init(NULL);
  if (ret < 0) {
    perror("libusb_init: Failed to initialize libusb");
    exit(1);
  }

  if ((udev = usb_device_find_USB_MCC(USB1208FS_PID, NULL))) {
    		//printf("USB-1208FS Device is found!\n");
    		init_USB1208FS(udev);
  } else {
    		printf("No device found.\n");
    		exit(0);
  }

  //get the wMaxPacketSize.  Should be 64
  packetSize[0] = usb_get_max_packet_size(udev,0);

  /* config mask 0x01 means all inputs */
  usbDConfigPort_USB1208FS(udev, DIO_PORTA, DIO_DIR_OUT);
  usbDConfigPort_USB1208FS(udev, DIO_PORTB, DIO_DIR_IN);
  usbDOut_USB1208FS(udev, DIO_PORTA, 0);
  usbDOut_USB1208FS(udev, DIO_PORTA, 0);		// its twice in reference, why?

  gain =  SE_10_00V;
  flag = fcntl(fileno(stdin), F_GETFL);
  fcntl(0, F_SETFL, flag | O_NONBLOCK);

  for(i=0;i<8;i++){
	channel = (uint8_t) (i);
	svalue = usbAIn_USB1208FS(udev, channel, gain);
	aVal[i] = (volts_SE(svalue) * calGain[i]) - dcOffset[i];
	T[i] = -276.0 + (aVal[i] * 118.0);
  }
  		
  fcntl(fileno(stdin), F_SETFL, flag);   
 
  /*libusb_clear_halt(udev, LIBUSB_ENDPOINT_IN | 1);
  libusb_clear_halt(udev, LIBUSB_ENDPOINT_OUT| 2);
  libusb_clear_halt(udev, LIBUSB_ENDPOINT_IN | 3);
  libusb_clear_halt(udev, LIBUSB_ENDPOINT_IN | 4);
  libusb_clear_halt(udev, LIBUSB_ENDPOINT_IN | 5);
  for (i = 0; i < 4; i++) {
	libusb_release_interface(udev, i);
  }*/
  libusb_close(udev);
  return( 0 );
}

int main()
{
  float tVal[8];
  int error, packSize;

  error = fetchTemperature( &packSize, tVal );

  printf("Packet size = %d\n",packSize);
  printf("%+02.2f   %+02.2f   %+02.2f   %+02.2f   %+02.2f   %+02.2f   %+02.2f   %+02.2f \n", tVal[0],tVal[1],tVal[2],tVal[3],tVal[4],tVal[5],tVal[6],tVal[7]);	

  return(error);
}
