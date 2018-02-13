#!/usr/bin/python
#
#
# Mechmine LLC (GmbH) ("COMPANY") CONFIDENTIAL
# Copyright (c) 2012-2015 Mechmine LLC, All Rights Reserved.
#
# NOTICE:  All information contained herein is, and remains the property of COMPANY. 
# The intellectual and technical concepts contained herein are proprietary to COMPANY 
# and may be covered by Swiss and Foreign Patents, patents in process, and are protected 
# by trade secret or copyright law.
# Dissemination of this information or reproduction of this material is strictly forbidden 
# unless prior written permission is obtained from COMPANY. Access to the source code 
# contained herein is hereby forbidden to anyone except current COMPANY employees, managers 
# or contractors who have executed Confidentiality and Non-disclosure agreements explicitly 
# covering such access.
#
# The copyright notice above does not evidence any actual or intended publication or disclosure  
# of this source code, which includes information that is confidential and/or proprietary, and 
# is a trade secret, of  COMPANY. ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
# OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT THE EXPRESS WRITTEN CONSENT OF 
# COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE LAWS AND INTERNATIONAL TREATIES.  
# THE RECEIPT OR POSSESSION OF THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY 
# ANY RIGHTS TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL 
# ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
#
__author__ = "R. Tanner, N. Eng"
__copyright__ = "Copyright (c) 2012-2018 Mechmine LLC"
__credits__ = [""]
__license__ = "prop"
__maintainer__ = "R. Tanner"
__email__ = "rt@mechmine.com"
__status__ = "ProdDevelopment"					# "Development", "Test", "Production"
__purpose__ = "IO to USB-1208 data logger for 8 channels."
#------------------------------------------------------------------------------
from Logger import Logger

import ctypes
import os

sensorLibraryDir  = os.path.dirname(os.path.abspath(__file__))
sensorLibraryPath = os.path.join(sensorLibraryDir, 'fetch1208temp.so')
sensorLibrary     = ctypes.cdll.LoadLibrary(sensorLibraryPath)

class TEMP1208(object):

	def fetch(self):
		import numpy

		fetchData = sensorLibrary.fetchTemperature
		fetchData.rettypes = ctypes.c_int
		fetchData.argtypes = [
			numpy.ctypeslib.ndpointer(numpy.int, flags="C_CONTIGUOUS"),
			numpy.ctypeslib.ndpointer(numpy.float32, flags="C_CONTIGUOUS")
		]
		packetSize = numpy.array(1, numpy.int)
		temperatures = numpy.empty(8, numpy.float32)

		error = fetchData( packetSize, temperatures )
		if error < 0:
			Logger.println('Error fetching temparature data (error code = ' + str(error) + ')', 'sensor', Logger.VERBOSITY_ERROR)
			return numpy.empty(0, numpy.float32)

		return temperatures,packetSize

#
# Main - ONLY FOR TEST
#
if __name__ == '__main__':
	import SimpleConfigParser
	import datetime
	import os.path
	import sys

	# Select configuration file
	configFilename   = None
	workingDirectory = os.getcwd()

	# Initialize sensor
	sensor = TEMP1208()
	tempVoltage, packetSize = sensor.fetch()

	print"USB packet size =     ", packetSize
	print str(datetime.datetime.now())+"  Temperatures:  "+str(format(tempVoltage[0],'.2f'))+"  "+str(format(tempVoltage[1],'.2f'))+"  "+str(format(tempVoltage[2],'.2f'))+"  "+str(format(tempVoltage[3],'.2f'))+"  "+str(format(tempVoltage[4],'.2f'))+"  "+str(format(tempVoltage[5],'.2f'))+"  "+str(format(tempVoltage[6],'.2f')) +"  "+str(format(tempVoltage[7],'.2f')) 
		
