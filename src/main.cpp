#include <iostream>
#include <string>
#include "protocolHandler.hpp"
#include "UDPSocket.hpp"

// 142.93.184.175:7033

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
        cout << "Handshake bem sucedido" << endl;
    else
        cout << "Não foi possível estabelecer conexão" << endl;

    string msg_transmitir = "Opa, como vai?";

    vector<char> msg ; 

    for(auto a : msg_transmitir) msg.push_back(a) ; 

    if(pc.sendData(socket, msg)) cout << "Mensagem enviada com sucesso\n" ; 
    else cout << "algum erro ocorreu\n" ; 

    return 0;
}
