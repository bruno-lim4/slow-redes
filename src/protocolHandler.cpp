#include "protocolHandler.hpp"
#include <iostream>

using namespace std; 

#define MAX_PACK_DATA 1440

//verifica se a conexão foi estabelecida - usado na main 
bool ProtocolHandler::verifyConenction() {
    return this->conn.isEstablished();
}

// Obtém o connection interno da comunicação
bool ProtocolHandler::verifyConnectionClosed() {
    return this->conn.isClosed();
}

//faz o 3way handshake 
bool ProtocolHandler::handshake(UDPSocket &socket, const string &ip, uint16_t port){
    
    //estabelecendo a conexão 
    if(!socket.connectTo(ip, port)){
        //não foi bem sucedida 
        cout << socket.getLastError() << "\n" ; 
        return false; 
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
    
    //ver quantos caracteres quero gravar
    // byte atual
    int at = 0;

    // vetor de pacotes (pode haver fragemtacao) + tamanho da mensagem 
    vector<Package> packages;

    // enquanto tiver algum byte sem pacote...
    while(total_left > 0) {
        Package pack;
        vector<char> aux;
        int sz = MAX_PACK_DATA ;

        // se couber tudo
        if (MAX_PACK_DATA >= total_left) {
            aux.insert(aux.end(), data.begin()+at, data.begin()+at+total_left);
            sz = min(sz, total_left);
            total_left -= total_left ;
        } else { // se nao couber
            // grava no max MAX_PACK_DATA
            aux.insert(aux.end(), data.begin()+at, data.begin()+at+MAX_PACK_DATA);
            // avanca
            at += MAX_PACK_DATA;
            total_left -= MAX_PACK_DATA;
        }

        // coloca aux como data no meu package
        pack.setData(aux);
        // coloca esse pack na minha lista de packages
        packages.push_back(pack);
    }

    // se ocorreu fragmentaco
    if (packages.size() > 1) {
        // seta a flag MB, e as info de fid e fo como devem ser setadas
        for(uint8_t i = 0; i < (uint8_t) packages.size(); i++) {
            (packages[i]).setFlageMB(true);
            (packages[i]).setFid(this->fid);
            (packages[i]).setFo(i);
        }
        (packages[packages.size() - 1]).setFlageMB(false);
        this->fid += 1;
    }

    for(auto p : packages) {
        conn.handleOutput(p, 1);
    
        //arrumar o seqnum 
        socket.send(p.serialize());
        Package ack = receiveLoop(socket, p.getSeqnum());
        conn.handleIncoming(ack);
    }

    // retorna se a conexao foi estabelecida
    return conn.isEstablished();
}

//reviver uma conexão encerrada
bool ProtocolHandler::sendRevive(UDPSocket &socket) {
    // Gerando um pacote revive
    Package pack;
    conn.handleOutput(pack, 4); 
    
    //envia o pacote 
    socket.send(pack.serialize()); 
    Package ack = receiveLoop(socket, pack.getSeqnum());
    conn.handleIncoming(ack);

    //retorna se conseguiu reestabelecer a conexão 
    return conn.isEstablished();
}



/* 
Aguarda até receber um pacote 
que o acknum seja o valor que estou esperando
*/
Package ProtocolHandler::receiveLoop(UDPSocket &socket, uint32_t ackEsperado){
    for(int i = 0 ; i < 3 ; i++){
        auto resp = socket.receive(1000) ;
        if(resp.has_value()){
            Package p = Package::deserialize(resp.value());

            //recebi o pacote que esperava
            if(p.getAcknum() == ackEsperado)
                return p;     
        } 
    }
    return {};
}

//efetuar o disconnect: 
// envio um pacote pra desconectar e recebo ack dele 
bool ProtocolHandler::sendDisconnect(UDPSocket &socket){
    Package disconnectPc;
    conn.handleOutput(disconnectPc, 3); 
    
    //enviando o pacote pra desconectar 
    socket.send(disconnectPc.serialize());

    //recebi um pacote ack pra confirmar o disconnect
    Package ack = receiveLoop(socket, 0);
    conn.handleIncoming(ack); 

    //verifico se agora a conexão está encerrada
    return !conn.isEstablished();
}
