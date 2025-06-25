#include "protocolHandler.hpp"
#include "package.hpp"
#include <iostream>


using namespace std; 

#define MAX_PACK_DATA 1440

bool ProtocolHandler::handshake(UDPSocket &socket, const string &ip, uint16_t port){
    
    //estabelecendo a conexão 
    if(!socket.connectTo(ip, port)){
        cout << ip << " " << port << endl;
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
    cout << connectPc.toString() << "\n";

    // recebe resposta
    Package resposta = ProtocolHandler::receiveLoop(socket, connectPc.getSeqnum());
    cout << resposta.toString() << "\n";
    // mando pro connection
    conn.handleIncoming(resposta); 
    // connection altera esse pacote pra simbolizar um envio com ACK
    conn.handleOutput(resposta, 1);

    // envio ACK sem dados
    auto envio2 = resposta.serialize();
    cout << resposta.toString() << "\n";
    socket.send(envio2) ; 

    // recebo uma resposta
    Package ack_com_dados = ProtocolHandler::receiveLoop(socket, resposta.getSeqnum());
    cout << ack_com_dados.toString() << "\n";
    // falo pra conection interpretar a resposta
    conn.handleIncoming(ack_com_dados); 

    // retorno se a conexao foi estabelecida
    return conn.isEstablished();
}

bool ProtocolHandler::sendData(UDPSocket &socket, const vector<char> &data){
    // ve quantos bytes
    int total_left = data.size();
    //ver quantos caracteres quero gravar
    cout << "tamanho " << total_left << "\n" ; 
    // byte atual
    int at = 0;

    // vetor de pacotes (pode haver fragemtacao) + tamanho da mensagem 
    vector<pair<Package, int>> packages;

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
            //checar se é MAX_PACK_DATA-1 ou não (maybe)
            aux.insert(aux.end(), data.begin()+at, data.begin()+at+MAX_PACK_DATA);
            // avanca
            at += MAX_PACK_DATA;
            total_left -= MAX_PACK_DATA;
        }

        // colcoa aux como data no meu package
        pack.setData(aux);
        // coloca esse pack na minha lista de packages
        packages.push_back({pack, sz});
    }

    // se ocorreu fragmentaco
    if (packages.size() > 1) {
        // seta a flag MB, e as info de fid e fo como devem ser setadas
        for(int i = 0; i < (int) packages.size() - 1; i++) {
            (packages[i].first).setFlageMB(true);
            (packages[i].first).setFid(fid);
            (packages[i].first).setFo(i);
        }
        fid += 1;
    }

    // envia geral - e vai checando os ACKs recebidos
    int new_seq = 0 ; 

    for(auto p : packages) {
        conn.handleOutput(p.first, 1);
        //arrumar o seqnum 
        //eh pra somar o payload? Se sim descomentar:
        //(p.first).setSeqnum((p.first).getSeqnum()+new_seq) ;
        (p.first).printAll() ;
        vector<char> test = p.first.serialize();
        Package::printBufferBits(test);

        socket.send((p.first).serialize());
        Package ack = receiveLoop(socket, (p.first).getSeqnum());
        ack.printAll();
        conn.handleIncoming(ack);
        new_seq += p.second ; 
    }

    // retorna se a conexao ainda ta ativa
    return conn.isEstablished();
}

/* 
Aguarda até receber um pacote 
que o acknum seja o valor que estou esperando
*/

Package ProtocolHandler::receiveLoop(UDPSocket &socket, uint32_t ackEsperado){
    for(int i = 0 ; i < 3 ; i++){
        auto resp = socket.receive(1000) ;

        Package::printBufferBits(resp.value());

        if(resp.has_value()){
            Package p = Package::deserialize(resp.value());
            if(p.getAcknum() == ackEsperado) return p;
        } 
    }

    return {};
}