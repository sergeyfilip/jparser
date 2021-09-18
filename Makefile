#Makefile for compiling jtools
CC = g++
LD = g++

INCDIR = -I./include 
CFLAGS = -g3 -Wall -c $(INCDIR) -std=c++11
LDFLAG = -g3 -Wall -std=c++11  
LDLIB  = -ljsoncpp /usr/lib64/libcurl.so.4 -lconfig++



all: jparser jreport janalyzer jmapper jgenerator jupdater jparser jinteger jmaker

jparser : main.o  item.o  rule_item.o config.o 
	$(LD) $(LDFLAG) $^ -o $@ $(LDLIB) 

jreport : jreport.o  item.o  rule_item.o device.o devices.o reportitem.o config.o 
	$(LD) $(LDFLAG) $^ -o $@ $(LDLIB)
	
janalyzer : janalyzer.o  item.o  rule_item.o config.o 
	$(LD) $(LDFLAG) $^ -o $@ $(LDLIB)

jmapper : jmapper.o  item.o  rule_item.o device.o devices.o config.o 
	$(LD) $(LDFLAG) $^ -o $@ $(LDLIB)
	
jgenerator : jgenerator.o  group.o  groups.o policy.o device.o devices.o policies.o reportitem.o helper.o config.o item.o
	$(LD) $(LDFLAG) $^ -o $@ $(LDLIB)
	
jupdater : updater.o config.o item.o
	$(LD) $(LDFLAG) $^ -o $@ $(LDLIB)
	
jinteger : jinteger.o helper.o item.o
	$(LD) $(LDFLAG) $^ -o $@ $(LDLIB)	
	
jmaker : jmaker.o item.o config.o helper.o
	$(LD) $(LDFLAG) $^ -o $@ $(LDLIB)	
	
main.o : main.cpp    
	$(CC) $(CFLAGS) $<
	
jreport.o : jreport.cpp    
	$(CC) $(CFLAGS) $<
	
janalyzer.o : janalyzer.cpp    
	$(CC) $(CFLAGS) $<

jmapper.o : jmapper.cpp
	$(CC) $(CFLAGS) $<
	
config.o : config.cpp ./include/config.h
	$(CC) $(CFLAGS) $<	

group.o : group.cpp ./include/group.h
	$(CC) $(CFLAGS) $<
	
updater.o : updater.cpp    
	$(CC) $(CFLAGS) $<			
	
groups.o : groups.cpp ./include/groups.h
	$(CC) $(CFLAGS) $<
	
policy.o : policy.cpp  ./include/policy.h
	$(CC) $(CFLAGS) $<
	
policies.o : policies.cpp ./include/policies.h
	$(CC) $(CFLAGS) $<	
	
device.o : device.cpp ./include/device.h
	$(CC) $(CFLAGS) $<

devices.o : devices.cpp ./include/devices.h
	$(CC) $(CFLAGS) $<
	
reportitem.o : reportitem.cpp ./include/reportitem.h
	$(CC) $(CFLAGS) $<

rule_item.o : rule_item.cpp ./include/rule_item.h
	$(CC) $(CFLAGS) $<
	
item.o : item.cpp ./include/item.h
	$(CC) $(CFLAGS) $<
	
helper.o : helper.cpp ./include/helper.h
	$(CC) $(CFLAGS) $<
	
jgenerator.o : jgenerator.cpp
	$(CC) $(CFLAGS) $<
	
jinteger.o : jinteger.cpp
	$(CC) $(CFLAGS) $<
	
jmaker.o : jmaker.cpp
	$(CC) $(CFLAGS) $<	
	
install : jparser jreport janalyzer jmapper jgenerator jinteger jmaker full.json ja3fingerprint.json categories.txt categories_orig.txt
	mkdir -p /etc/jparser
	cp full.json /etc/jparser
	cp ja3fingerprint.json /etc/jparser
	cp categories.txt /etc/jparser
	cp categories_orig.txt /etc/jparser
	cp jparser /usr/bin
	cp jreport /usr/bin
	cp janalyzer /usr/bin
	cp jmapper /usr/bin
	cp jgenerator /usr/bin
	cp jinteger /usr/bin
	cp jmaker /usr/bin


clean :   
	rm -f jreport jparser janalyzer jmapper jgenerator jinteger jmaker jupdater config.o jgenerator.o main.o item.o rule_item.o jreport.o janalyzer.o reportitem.o device.o devices.o jmapper.o group.o groups.o policy.o policies.o helper.o jinteger.o jmaker.o updater.o
	
	
