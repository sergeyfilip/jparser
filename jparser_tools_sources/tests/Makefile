#Makefile for compiling jtools
CC = g++
LD = g++

INCDIR = -I./include 
CFLAGS = -g3 -Wall -c $(INCDIR) -std=c++11
LDFLAG = -g3 -Wall -std=c++11  
LDLIB  = -ljsoncpp /usr/lib64/libcurl.so.4



all: test

test : test.o 
	$(LD) $(LDFLAG) $^ -o $@ $(LDLIB) 

test.o : test.cpp    
	$(CC) $(CFLAGS) $<
	
clean :   
	rm -f test test.o
	
	
