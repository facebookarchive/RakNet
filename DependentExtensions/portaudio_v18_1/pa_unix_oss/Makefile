# Make PortAudio for Linux
#
# by Phil Burk
#
# To compile a test program, make a target with a .app suffix.
# For example to compile "../pa_tests/pa_devs.c", enter:
#    gmake pa_devs.app
#
# To compile and execute a test program, make a target with a .run suffix.
# For example to build and run "../pa_tests/pa_devs.c", enter:
#    gmake pa_devs.run

VPATH  = ../pa_common ../pa_tests
CFLAGS = -g -Wall -I../pa_common
CC     = gcc

PAOBJS = pa_lib.o pa_unix_oss.o pa_unix.o
PAINC  = portaudio.h pa_unix.h
PALIBS =  -lm -lpthread

all: patest_sine.run

%.app: %.o $(PAOBJS) $(PAINC) Makefile
	gcc $(CFLAGS) $*.o $(PAOBJS) $(PALIBS) -o $@

%.run: %.app
	./$*.app

clean:
	-rm *.app
	-rm *.o

