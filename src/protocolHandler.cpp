#include "protocolhandler.hpp"
#include <iostream>
#include <bits/stdc++.h>

using namespace std ; 

bool ProtocolHandler::Handshake(UDPSocket &socket, const string &ip, uint16_t port){
    //estabelecendo a conexão 
    if(!socket.connectTo(ip, port)){
        //não foi bem sucedida 
        cout << socket.getLastError() << "\n" ; 
        return 0 ; 
    }
    
    // abre conexao
    Connection conn = new Connection();

    // cria package de connect - e informa ao connection
    Package connectPc = new Package();
    conn.handleOutput(connectPc, 2);
    
    // envia connect
    auto envio = Package::connectPc.serialize();
    UDPSocket::socket.send(envio) ; 

    // recebe resposta
    ProtocolHandler::receiveLoop(socket, envio.getSeqNum())
    auto resposta = UDPSocket::socket.receive(10000) ; 

    if(!resposta.has_value()){//chegou algum dado? 
        cout << "Timeout agurdando resposta do handshake\n" ; 
        return false ; 
    }


    Package ack_sem_dados; 
    ack_sem_dados.deserialize(resposta.value()) ; 
    conn.handleIncoming(ack_sem_dados); 

    conn.handleOutput(ack_sem_dados, 1);

    // envio ACK sem dados
    auto envio = Package::connectPc.serialize();
    UDPSocket::socket.send(envio) ; 

    auto resposta2 = UDPSocket::socket.receive(10000) ; 

    if(!resposta2.has_value()){//chegou algum dado? 
        cout << "Timeout agurdando resposta do handshake\n" ; 
        return false ; 
    }

    Package ack_com_dados;

    ack_com_dados.deserialize(resposta2.value()) ; 
    conn.handleIncoming(ack_com_dados); 

    // espero que a conexao foi estabelecida

    return ;
}

bool ProtocolHandler::sendData(UDPSocket &socket, const vector<char> &data){

}

/*
Aguarda até receber um pacote 
que o acknum seja o valor que estou esperando
*/

void ProtocolHandler::receiveLoop(UDPSocket &socket, uint32_t Ackesperado){
    for(int i = 0 ; i < 3 ; i++){
        auto resp = socket.receive(1000) ;
        if(resp.has_value()){
            Package p ; 
            p.deserialize(resp.value()) ; 
            if(p.getAcknum() == Ackesperado) break ; 
        } 
    }
}