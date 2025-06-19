FLAGS = -std=c++17 -Wall -g

all: peripheral

peripheral: main.o UDPSocket.o package.o connection.o protocolHandler.o
	g++ $(FLAGS) *.o -o peripheral

main.o: main.cpp
	g++ $(FLAGS) -c main.cpp

protocolHandler.o: UDPSocket.o package.o connection.o protocolHandler.cpp protocolHandler.hpp
	g++ $(FLAGS) -c protocolHandler.cpp

connection.o: connection.cpp connection.hpp
	g++ $(FLAGS) -c connection.cpp

package.o: package.cpp package.hpp
	g++ $(FLAGS) -c package.cpp

UDPSocket.o: UDPSocket.cpp UDPSocket.hpp
	g++ $(FLAGS) -c UDPSocket.cpp


clean:
	rm -f *.o peripheral

run: peripheral
	./peripheral
