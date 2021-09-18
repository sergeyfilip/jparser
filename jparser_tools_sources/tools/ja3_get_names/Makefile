getnames : main.o item.o
	g++ -std=c++11 -o getnames  main.o item.o -ljsoncpp /usr/lib64/libcurl.so.4 -g 
main.o : main.cpp 
	g++ -std=c++11 -c main.cpp 
item.o : item.cpp item.h
	g++ -std=c++11 -c item.cpp 
clean :   
	rm  main.o
