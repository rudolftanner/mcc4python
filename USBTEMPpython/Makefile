#############################################################################
#	                                                                    #
#	Makefile for building:                                              #
#                                                                           #
#		libmccusb.so:        Library for USB series                 #
#                                                                           #
#                                                                           #
#                                                                           #
#               Copyright (C)  2014-2016                                    #
#               Written by:  Warren J. Jasper                               #
#                            wjasper@ncsu.edu                               #
#                            North Carolina State Univerisity               #
#                                                                           #
#############################################################################

#  Current Version of the driver
VERSION=1.18

SRCS =    pmd.c nist.c usb-temp.c fetchUSBtemp.c
         
HEADERS = pmd.h usb-temp.h
          
OBJS = $(SRCS:.c=.o)   # same list as SRCS with extension changed
CC=gcc
CFLAGS+= -g -Wall -fPIC -O -I/usr/local/include/libusb-1.0 -L/usr/local/lib -lusb-1.0
ifeq ($(shell uname), Darwin)
	SONAME_FLAGS = -install_name
	SHARED_EXT = dylib
else
	SONAME_FLAGS = -soname
	SHARED_EXT = so
endif 
TARGETS=libmccusb.$(SHARED_EXT) libmccusb.a test-usb-temp fetchUSBtemp
        
ID=MCCLIBUSB
DIST_NAME=$(ID).$(VERSION).tgz
DIST_FILES={README,Makefile,nist.c,pmd.c,pmd.h,usb-temp.h,usb-temp.c,test-usb-temp.c,fetchUSBtemp.c}

###### RULES
all: $(TARGETS)

%.d: %.c
	set -e; $(CC) -I. -M $(CPPFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@
ifneq ($(MAKECMDGOALS),clean)
include $(SRCS:.c=.d)
endif

libmccusb.$(SHARED_EXT): $(OBJS)
#	$(CC) -O -shared -Wall $(OBJS) -o $@
	$(CC) -shared -Wl,$(SONAME_FLAGS),$@ -o $@ $(OBJS) -lc -lm $(CFLAGS)

libmccusb.a: $(OBJS)
	ar -r libmccusb.a $(OBJS)
	ranlib libmccusb.a

#
# libusb-1.0
#

################ HID devices ##########################
test-usb-temp: test-usb-temp.c usb-temp.o libmccusb.a 
	$(CC) -g -Wall -I. -o $@ $@.c -L. -lmccusb  -lm -L/usr/local/lib  -lhidapi-libusb -lusb-1.0 

fetchUSBtemp: fetchUSBtemp.c fetchUSBtemp.o libmccusb.a
	$(CC) -g -Wall -I. -o $@ $@.c -L. -lmccusb  -lm -L/usr/local/lib  -lhidapi-libusb -lusb-1.0 
	$(CC) -shared -o fetchUSBtemp.so -lpthread fetchUSBtemp.o libmccusb.a -lhidapi-libusb -lusb-1.0
####################################################################################################

clean:
	rm -rf \#*\# *.d *.o *~ *.a *.so *.dylib *.dll *.lib *.dSYM $(TARGETS)

dist:	
	make clean
	cd ..; tar -zcvf $(DIST_NAME) mcc-libusb/$(DIST_FILES);

install:
	-install -d /usr/local/lib
	-install -c --mode=0755 ./libmccusb.a libmccusb.$(SHARED_EXT) /usr/local/lib
	-/bin/ln -s /usr/local/lib/libmccusb.$(SHARED_EXT) /usr/lib/libmccusb.$(SHARED_EXT)
	-/bin/ln -s /usr/local/lib/libmccusb.a /usr/lib/libmccusb.a
	-install -d /usr/local/include/libusb
	-install -c --mode=0644 ${HEADERS} /usr/local/include/libusb/

uninstall:
	-rm -f /usr/local/lib/libmccusb*
	-rm -f /usr/lib/libmccusb*
	-rm -rf /usr/local/include/libusb
