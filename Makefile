# Makefile for building C stuff with GSL
DIR=/.


BASE=$(bleung)/C++/Copper/Copper
DIR2=/usr/include/gsl
LDFLAGS=-lgsl -lgslcblas

DIR1=$(BASE)/LIBRARY2
CC=g++
#ARCH=-march=native -mfpmath=sse
ARCH=-mfpmath=sse
CFLAGS=-I$(DIR2) -I$(DIR1) -w -g $(ARCH)

SOURCES= copper.cpp output.cpp init.cpp $(DIR1)/vbc_FileManagement.cpp $(DIR1)/vbc_stringfuncs.cpp
#OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=ex_copper
#%: %.c
#        $(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE) $(LDFLAGS)

#$(EXECUTABLE): $(OBJECTS)
#	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS) 
#.cpp.o:
#	$(CC) $(CFLAGS) $< -o $@
#
#.c.o:
#	$(CC) $(CFLAGS) $< -o $@
	
	
# eg. do "make gsl_test" to make gsl_test from gsl_test.c
# then run with "gsl_test 10"

clean:
	rm -f *~ *.o core a.out
