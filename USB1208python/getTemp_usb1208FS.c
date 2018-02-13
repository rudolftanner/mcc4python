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

int main (int argc, char **argv)
{
  int flag;
  signed short svalue;
  uint8_t channel, gain;
  //uint8_t gains[8];
  //int nScan;
  //uint16_t value;
  //uint16_t out_data[512];
  //signed short in_data[1024];
  //int count;
  int i;
  //int ch;
  float aVal[8], tVal[8];
  float calGain[8]  = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  float dcOffset[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.01, 0.015, 0.0};
  //time_t startTime, endTime;

  libusb_device_handle *udev = NULL;
  int ret;
  //unsigned char serial[9];
  
  ret = libusb_init(NULL);
  if (ret < 0) {
    perror("libusb_init: Failed to initialize libusb");
    exit(1);
  }

  if ((udev = usb_device_find_USB_MCC(USB1208FS_PID, NULL))) {
    printf("USB-1208FS Device is found!\n");
    init_USB1208FS(udev);
  } else {
    printf("No device found.\n");
    exit(0);
  }

  //print out the wMaxPacketSize.  Should be 64
  printf("wMaxPacketSize = %d\n", usb_get_max_packet_size(udev,0));

  /* config mask 0x01 means all inputs */
  usbDConfigPort_USB1208FS(udev, DIO_PORTA, DIO_DIR_OUT);
  usbDConfigPort_USB1208FS(udev, DIO_PORTB, DIO_DIR_IN);
  usbDOut_USB1208FS(udev, DIO_PORTA, 0);
  usbDOut_USB1208FS(udev, DIO_PORTA, 0);

  //usbReset_USB1208FS(udev);

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  FILE *f = fopen("tempLogfile.txt", "w+");
  if (f == NULL){
    printf("Error opening file!\n");
    exit(1);
  }
  printf("Timestamp: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec );
  fprintf(f, "Timestamp: %d-%d-%d %02d:%02d:%02d\n",  tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec );
  fclose(f);

  printf("\nStart measurement...\n");
  while(1) {
       
	gain =  SE_10_00V;

        flag = fcntl(fileno(stdin), F_GETFL);
        fcntl(0, F_SETFL, flag | O_NONBLOCK);

        do {
		for(i=0;i<8;i++){
			channel = (uint8_t) (i);
	  		svalue = usbAIn_USB1208FS(udev, channel, gain);
			aVal[i] = (volts_SE(svalue) * calGain[i]) - dcOffset[i];
			tVal[i] = -276.0 + (aVal[i] * 118.0);
		}
		printf("Measurement: ");
		for(i=0;i<8;i++){
			printf("%+02.3fV  ", aVal[i] );
		}
		printf("\nTemperature: ");
		for(i=0;i<8;i++){
			printf("%+02.2fC  ", tVal[i] );
		}
		printf(" \n ");

		FILE *f = fopen("tempLogfile.txt", "a+");
  		if (f == NULL){
    			printf("Error opening file!\n");
    			exit(1);
  		}
		fprintf(f, "%+02.2f   %+02.2f   %+02.2f   %+02.2f   %+02.2f   %+02.2f   %+02.2f   %+02.2f  \n", tVal[0],tVal[1],tVal[2],tVal[3],tVal[4],tVal[5],tVal[6],tVal[7]);
		fclose(f);

		sleep(30);

	} while (!isalpha(getchar()));
	fcntl(fileno(stdin), F_SETFL, flag);
	break;
    	}

  libusb_clear_halt(udev, LIBUSB_ENDPOINT_IN | 1);
  libusb_clear_halt(udev, LIBUSB_ENDPOINT_OUT| 2);
  libusb_clear_halt(udev, LIBUSB_ENDPOINT_IN | 3);
  libusb_clear_halt(udev, LIBUSB_ENDPOINT_IN | 4);
  libusb_clear_halt(udev, LIBUSB_ENDPOINT_IN | 5);
  for (i = 0; i < 4; i++) {
	libusb_release_interface(udev, i);
  }
  libusb_close(udev);
}
