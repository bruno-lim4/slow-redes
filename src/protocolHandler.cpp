#include "protocolHandler.hpp"
#include <iostream>
#include <bits/stdc++.h>

using namespace std; 

#define MAX_PACK_DATA 1440

bool ProtocolHandler::handshake(UDPSocket &socket, const string &ip, uint16_t port){
    //estabelecendo a conexão 
    if(!socket.connectTo(ip, port)){
        //não foi bem sucedida 
        cout << socket.getLastError() << "\n" ; 
        return 0 ; 
    }

    // cria package de connect - e informa ao connection
    Package connectPc;
    conn.handleOutput(connectPc, 2);
    
    // envia connect
    auto envio = connectPc.serialize();
    socket.send(envio) ; 

    // recebe resposta
    Package resposta = ProtocolHandler::receiveLoop(socket, connectPc.getSeqnum());
    // mando pro connection
    conn.handleIncoming(resposta); 
    // connection altera esse pacote pra simbolizar um envio com ACK
    conn.handleOutput(resposta, 1);

    // envio ACK sem dados
    auto envio2 = resposta.serialize();
    socket.send(envio2) ; 

    // recebo uma resposta
    Package ack_com_dados = ProtocolHandler::receiveLoop(socket, resposta.getSeqnum());

    // falo pra conection interpretar a resposta
    conn.handleIncoming(ack_com_dados); 

    // retorno se a conexao foi estabelecida
    return conn.isEstablished();
}

bool ProtocolHandler::sendData(UDPSocket &socket, const vector<char> &data){
    // ve quantos bytes
    int total_left = data.size();
    // byte atual
    int at = 0;

    // vetor de pacotes (pode haver fragemtacao)
    vector<Package> packages;

    // enquanto tiver algum byte sem pacote...
    while(total_left > 0) {
        Package pack;

        vector<char> aux;
        int qt;

        // se caber tudo
        if (MAX_PACK_DATA >= total_left) {
            aux.insert(aux.end(), data.begin()+at, data.begin()+at+(total_left-1));
            total_left -= total_left;
        } else { // se nao caber
            // grava no max MAX_PACK_DATA
            aux.insert(aux.end(), data.begin()+at, data.begin()+at+(MAX_PACK_DATA-1));
            // avanca
            at += MAX_PACK_DATA;
            total_left -= MAX_PACK_DATA;
        }

        // colcoa aux como data no meu package
        pack.setData(aux);
        // coloca esse pack na minha lista de packages
        packages.push_back(pack);
    }

    // se ocorreu fragmentaco
    if (packages.size() > 1) {
        // seta a flag MB, e as info de fid e fo como devem ser setadas
        for(int i = 0; i < (int) packages.size() - 1; i++) {
            packages[i].setFlageMB(true);
            packages[i].setFid(fid);
            packages[i].setFo(i);
        }
        fid += 1;
    }

    // envia geral - e vai checando os ACKs recebidos
    for(auto p : packages) {
        conn.handleOutput(p, 1);
        socket.send(p.serialize());
        Package ack = receiveLoop(socket, p.getSeqnum());
        conn.handleIncoming(ack);
    }

    // retorna se a conexao ainda ta ativa
    return conn.isEstablished();
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