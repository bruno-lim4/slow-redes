#include <iostream>
#include <string>
#include "protocolHandler.hpp"
#include "UDPSocket.hpp"

#define SERVER_IP "142.93.184.175"
#define SERVER_PORT 7033

// Interface de terminal para se comunicar com o central
void interface() {
    UDPSocket socket;
    ProtocolHandler pc;
    bool checkCom;

    int option;
    vector<char> message;
    string line;

    // Titulo
    cout << "|======  S.L.U.G  ======|" << endl;
    cout << "|Slow Little UDP Gateway|" << endl;
    cout << "|=======================|" << endl << endl;

    // Mensagem inicial
    cout << "*== Digite 0 para ver os comandos ==*" << endl << endl;

    // Loop de interação
    while(true) {
        cin >> option;

        switch(option) {
            case 0:        // Mostrar opções
                cout << "=== COMANDOS ===" << endl;
                cout << "0 - mostrar comandos" << endl;
                cout << "1 - Conectar com o servidor" << endl;
                cout << "2 - Enviar uma mensagem" << endl;
                cout << "3 - Desconectar" << endl;
                cout << "4 - Reviver conexão" << endl << endl;
                cout << "5 - Encerrar programa" << endl << endl;
            break;
        
            case 1:        // Conectar
                checkCom = pc.handshake(socket, SERVER_IP, SERVER_PORT);
                if(checkCom)
                    cout << "=== CONECTADO ===" << endl << endl;
                else
                    cout << "!== ERRO AO CONECTAR ==!" << endl << endl;
            break;

            case 2:        // Enviar mensagem
                if(!pc.verifyConenction()) {
                    cout << "*== É preciso ter conexão ativa para essa ação ==*";
                    break;
                }

                cout << "*== Digite sua mensagem ==*" << endl;
                getline(cin, line); // Consome o \n do input anterior;
                
                // Lendo a mensagem
                do {
                    getline(cin, line);

                    for(auto letter : line) {
                        message.push_back(letter);
                    }
                }while(line.size() != 0);

                if(pc.sendData(socket, message)) 
                    cout << "=== MENSAGEM ENVIADA ===" << endl << endl; 
                else 
                    cout << "!== ERRO NO ENVIO DA MENSAGEM ==!" << endl << endl; 
            break;

            case 3:         // Desconectar
                if(!pc.verifyConenction()) {
                    cout << "*== É preciso ter conexão ativa para essa ação ==*";
                    break;
                }

                checkCom = pc.sendDisconnect(socket);
                if(checkCom)
                    cout << "=== DESCONECTADO ===" << endl << endl;
                else
                    cout << "!== ERRO AO DESCONECTAR ==!" << endl << endl;
            break;
            case 4:        // Revive
                checkCom = pc.sendRevive(socket);
                if(checkCom)
                    cout << "=== CONEXÃO RECUPERADA ===" << endl << endl;
                else
                    cout << "!== ERRO AO RECUPERAR CONEXÃO ==!" << endl << endl;
            break;

            case 5:         // Encerra a interface
                if(pc.verifyConenction()) 
                    pc.sendDisconnect(socket);
                socket.disconnect();

                cout << "=== PROGRAMA ENCERRADO ===" << endl << endl;
                return;
            break;

            default:
                cout << "!== COMANDO INVALIDO ==!" << endl << endl;
            break;
        }
    }
}

int main() {
    interface();
    return 0;
}
