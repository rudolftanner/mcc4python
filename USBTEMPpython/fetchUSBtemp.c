/*
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
#include <fcntl.h>
#include <ctype.h>

#include "pmd.h"
#include "usb-temp.h"
#include "usb-1024LS.h"

#define MAX_STR 255

float volts_FS(const int gain, const signed short num);


/* Test Program */
float celsius2fahr( float celsius)
{
  return (celsius*9.0/5.0 + 32.);
}

float fahr2celsius( float fahr)
{
  return (fahr - 32.)*5.0/9.0;
}

int toContinue()
{
  int answer;
  answer = 0; //answer = getchar();
  printf("Continue [yY]? ");
  while((answer = getchar()) == '\0' ||
    answer == '\n');
  return ( answer == 'y' || answer == 'Y');
}

int fetchTemperature( float *tempVal )
{
  int ch;
  float temperature;
  int flag; 
  int ret;
  hid_device *hid = 0x0;  // Composite device with 1 interface.
  
  ret = hid_init();
  if (ret < 0) {
    fprintf(stderr, "hid_init failed with return code %d\n", ret);
    return -1;
  }

  if ((hid = hid_open(MCC_VID, USB_TEMP_PID, NULL)) < 0) {
    fprintf(stderr, "USB-TEMP not found.\n");
    exit(1);
  }

  /* config mask 0x01 means all inputs */
  usbDConfigPort_USBTEMP(hid, DIO_DIR_OUT);
  usbDOut_USBTEMP(hid, 0x0);
 
  for(ch=0;ch<8;ch++){
	
  	flag = fcntl(fileno(stdin), F_GETFL);
  	fcntl(fileno(stdin), F_SETFL, flag | O_NONBLOCK);
  
        usbTin_USBTEMP(hid, ch, 0, &temperature);
	
	tempVal[ch] = temperature;
	//printf(" %.2f   %.2f  \n", tempVal[ch], temperature);
  	sleep(0.25);
  }
  //printf(" %.2f   %.2f   %.2f   %.2f   %.2f   %.2f   %.2f   %.2f  \n", tempVal[0],tempVal[1],tempVal[2],tempVal[3],tempVal[4],tempVal[5],tempVal[6],tempVal[7]);
  fcntl(fileno(stdin), F_SETFL, flag);
  return(0);
}

int main()
{
  float tempVal[8];
  int error;

  error = fetchTemperature( tempVal );

  printf(" %.2f   %.2f   %.2f   %.2f   %.2f   %.2f   %.2f   %.2f  \n", tempVal[0],tempVal[1],tempVal[2],tempVal[3],tempVal[4],tempVal[5],tempVal[6],tempVal[7]);
}
