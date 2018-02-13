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

int main(int argc, char **argv)
{
  int ch;
  float temperature;
  int flag;
  //float R0, A, B, C;      // RTD: Callendar-Van Dusen coefficients
  float tempVal[8];
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

  // printf("Sampling RTD\n");
  //R0 = 100.;
  //A = .003908;
  //B = -5.8019E-7;
  //C = -4.2735E-12;
 
  for(ch=0;ch<8;ch++){
	/*usbSetItem_USBTEMP(hid, ch/2, CONNECTION_TYPE, TWO_WIRE_TWO_SENSOR); 

  	usbSetItem_USBTEMP(hid, ch/2, EXCITATION, MU_A_210);
 	usbSetItem_USBTEMP(hid, ch/2, CH_0_GAIN + ch%2, 0x2);          // Set 0 - 0.5V for RTD
  	usbSetItem_USBTEMP(hid, ch/2, CH_0_COEF_0 + ch%2, R0);         // R0 value
  	usbGetItem_USBTEMP(hid, ch/2, CH_0_COEF_0 + ch%2, &value);

  	//printf("R0 = %f     ", value);
  	usbSetItem_USBTEMP(hid, ch/2, CH_0_COEF_1 + ch%2, A);          // Callendar-Van Dusen Coefficient A
  	usbGetItem_USBTEMP(hid, ch/2, CH_0_COEF_1 + ch%2, &value);
  	//printf("A = %e     ", value);
  	usbSetItem_USBTEMP(hid, ch/2, CH_0_COEF_2 + ch%2, B);          // Callendar-Van Dusen Coefficient B
  	usbGetItem_USBTEMP(hid, ch/2, CH_0_COEF_2 + ch%2, &value);
  	//printf("B = %e     ", value);
  	usbSetItem_USBTEMP(hid, ch/2, CH_0_COEF_3 + ch%2, C);          // Callendar-Van Dusen Coefficient C
  	usbGetItem_USBTEMP(hid, ch/2, CH_0_COEF_3 + ch%2, &value);
  	//printf("C = %e\n", value);
	*/
  	flag = fcntl(fileno(stdin), F_GETFL);
  	fcntl(fileno(stdin), F_SETFL, flag | O_NONBLOCK);
  
        usbTin_USBTEMP(hid, ch, 0, &temperature);
  	//printf("Channel: %d  %.2f degress Celsius.\n",ch, temperature);
	tempVal[ch] = temperature;
  	sleep(0.25);
  }
  printf(" %.2f   %.2f   %.2f   %.2f   %.2f   %.2f   %.2f   %.2f  \n", tempVal[0],tempVal[1],tempVal[2],tempVal[3],tempVal[4],tempVal[5],tempVal[6],tempVal[7]);
  fcntl(fileno(stdin), F_SETFL, flag);
}
