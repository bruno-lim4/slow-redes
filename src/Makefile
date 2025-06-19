FLAGS = -std=c++17 -Wall -g

all: peripheral

peripheral: main.o UDPSocket.o package.o
	g++ $(FLAGS) *.o -o peripheral

main.o: main.cpp
	g++ $(FLAGS) -c main.cpp

#protocolHandler.o: protocolHandler.cpp protocolHandler.hpp connection.hpp segment.hpp UDPSocket.h
#	g++ $(FLAGS) -c protocolHandler.cpp

package.o: package.cpp package.hpp
	g++ $(FLAGS) -c package.cpp

UDPSocket.o: UDPSocket.cpp UDPSocket.h
	g++ $(FLAGS) -c UDPSocket.cpp


clean:
	rm -f *.o peripheral

run: peripheral
	./peripheral

###
##all: peripheral

#peripheral: main.o UDPSocket.o protocolHandler.o
 #   g++ $(FLAGS) main.o UDPSocket.o protocolHandler.o -o peripheral

#main.o: main.cpp
 #   g++ $(FLAGS) -c main.cpp

#UDPSocket.o: UDPSocket.cpp UDPSocket.h
 #   g++ $(FLAGS) -c UDPSocket.cpp
#
#protocolHandler.o: protocolHandler.cpp protocolHandler.hpp connection.hpp segment.hpp UDPSocket.h
 #   g++ $(FLAGS) -c protocolHandler.cpp
#
#clean:
 #   rm -f *.o peripheral

#run: peripheral
#	./peripheral*/