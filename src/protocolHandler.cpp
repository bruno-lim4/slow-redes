#include "protocolHandler.hpp"
#include <iostream>
#include <bits/stdc++.h>

using namespace std; 

bool ProtocolHandler::handshake(UDPSocket &socket, const string &ip, uint16_t port){
    //estabelecendo a conexão 
    if(!socket.connectTo(ip, port)){
        //não foi bem sucedida 
        cout << socket.getLastError() << "\n" ; 
        return 0 ; 
    }
    
    // abre conexao
    Connection conn;

    // cria package de connect - e informa ao connection
    Package connectPc;
    conn.handleOutput(connectPc, 2);
    
    // envia connect
    auto envio = connectPc.serialize();
    socket.send(envio) ; 

    // recebe resposta
    Package resposta = ProtocolHandler::receiveLoop(socket, connectPc.getSeqnum());

    conn.handleIncoming(resposta); 

    conn.handleOutput(resposta, 1);

    // envio ACK sem dados
    auto envio2 = resposta.serialize();
    socket.send(envio2) ; 

    Package ack_com_dados = ProtocolHandler::receiveLoop(socket, resposta.getSeqnum());

    conn.handleIncoming(ack_com_dados); 

    // espero que a conexao foi estabelecida

    return conn.isEstablished();
}

bool ProtocolHandler::sendData(UDPSocket &socket, const vector<char> &data){

}

/*
Aguarda até receber um pacote 
que o acknum seja o valor que estou esperando
*/

Package ProtocolHandler::receiveLoop(UDPSocket &socket, uint32_t Ackesperado){
    for(int i = 0 ; i < 3 ; i++){
        auto resp = socket.receive(1000) ;
        if(resp.has_value()){
            Package p ; 
            p.deserialize(resp.value()) ; 
            if(p.getAcknum() == Ackesperado) return p;
        } 
    }
}