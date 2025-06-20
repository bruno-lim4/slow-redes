#include <iostream>
#include <string>
#include "protocolHandler.hpp"
#include "UDPSocket.hpp"

// slow.gmelodie.com:7033

#define SERVER_IP "142.93.184.175"
#define SERVER_PORT 7033

int main() {
    const string serverIp = "142.93.184.175";
    uint16_t serverPort = 7033;

    UDPSocket socket;
    ProtocolHandler pc;
    bool checkCom;

    checkCom = pc.handshake(socket, SERVER_IP, SERVER_PORT);
    if(checkCom)
        cout << "Yay" << endl;
    else
        cout << "Tristeza" << endl;

    return 0;
}
