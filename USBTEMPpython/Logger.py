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
__author__ = "A. Iob"
__copyright__ = "Copyright (c) 2012-2015 Mechmine LLC"
__credits__ = [""]
__license__ = "prop"
__maintainer__ = "Rudi Tanner"
__email__ = "rt@mechmine.com"
__status__ = "Production"			# "Production", "Development", "Test"
__purpose__ = ""

"""@package Logger
The Logger package allows to print output messages to the screen with a
standard format.
"""
class Logger(object):
	VERBOSITY_NONE   = 0
	VERBOSITY_ERROR  = 1
	VERBOSITY_NORMAL = 2
	VERBOSITY_DEBUG  = 3

	__current_verbosity = VERBOSITY_NORMAL

	@classmethod
	def setVerbosity(cls, verbosity):
		"""
		Set the verbosity level.
		"""
		try:
			verbosity = int(verbosity)
		except ValueError:
			message = "Verbosity level \"" + str(verbosity) + "\" is not valid."
			cls.println(message, 'logger', cls.VERBOSITY_ERROR)
			return

		cls.__current_verbosity = verbosity

	@classmethod
	def getVerbosity(cls):
		"""
		Get the verbosity level.
		"""

		return cls.__current_verbosity

	@classmethod
	def println(cls, message, context=None, verbosity=VERBOSITY_NORMAL):
		"""
		Writes a message to screen.
		"""

		import threading
		import time

		if verbosity > cls.__current_verbosity:
			return

		# Timestamp
		now = time.localtime(time.time())
		timeStamp = time.strftime("%Y-%m-%d %H:%M:%S", now)

		# Meggase line
		line = '[ ' + timeStamp + ' ] '
		if context:
			line += '(' + context + ') '
		line += message

		# Print message
		lock = threading.Lock()
		with lock:
			print line
