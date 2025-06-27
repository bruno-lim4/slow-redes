#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H
 
#include <iostream>
#include <string>
#include <optional>
#include <sys/socket.h>  
#include <arpa/inet.h>    
#include <netinet/in.h>   
#include <unistd.h>       
#include <cstring>        
#include <cstdint> 
#include <cerrno>         
#include <vector> 

#define TAM_BUF 1472 

using namespace std ; 

class UDPSocket {
public:

    UDPSocket();
    ~UDPSocket();

    //“Conecta” este socket a um peer (armazena IP e porta internamente)
    //Internamente cria o socket UDP e configura timeout de 100 ms para recv
    //Retorna true em sucesso, false em caso de erro
    bool connectTo(const string& ip, uint16_t port);

    // Desconecta o socket em nível de kernel e libera o seu
    // descritor
    bool disconnect();

    //Envia um serializado via UDP (sendto)
    //Retorna número de bytes enviados, ou –1 em caso de erro
    ssize_t send(const vector<char>& segment);
    
    //Tenta receber um Segmento
    //Se não chegar nada em 100 ms ou ocorrer erro, retorna null.
    optional<vector<char>> receive(uint32_t timeout);

    //Se alguma chamada falhar, getLastError() devolve o errno da última operação.
    int getLastError() const;

private:
    int                sockfd_;      // descritor de socket UDP
    struct sockaddr_in peerAddr_;    // endereço do peer (IP + porta)
    bool               isConnected_; // true se connectTo() já foi bem sucedido
    int                lastErrno_;   // armazena errno em erro
};

#endif 
