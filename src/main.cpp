#include <iostream>
#include <string>
#include "protocolHandler.hpp"
#include "UDPSocket.hpp"

// slow.gmelodie.com:7033

#define SERVER_IP "slow.gmelodie.com"
#define SERVER_PORT 7033

int main() {
    const string serverIp = "slow.gmelodie.com";
    uint16_t serverPort = 7033;

    UDPSocket socket;
    ProtocolHandler pc;
    bool checkCom;


    if(!socket.connectTo(serverIp, serverPort))
        cout << "Erro" << endl;

    /*
    checkCom = pc.handshake(socket, SERVER_IP, SERVER_PORT);
    if(checkCom)
        cout << "Yay" << endl;
    else
        cout << "Tristeza" << endl;
    */
    return 0;
}