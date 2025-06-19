#ifndef PROTOCOLHANDLER_H
#define PROTOCOLHANDLER_H

    #include "UDPSocket.hpp"
    #include "connection.hpp"
    #include <iostream>
    #include <vector>
    #include <sstream>
    #include <cstdint>

    class ProtocolHandler
    {
    private:
        Package receiveLoop(UDPSocket &socket, uint32_t Ackesperado);
    public:
        bool handshake(UDPSocket &socket, const string &ip, uint16_t port);
        bool sendData(UDPSocket &socket, const vector<char> &data);
    };
    
#endif
