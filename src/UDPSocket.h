#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

#include <string>
#include <optional>

using namespace std ; 

class UDPSocket {
public:

    UDPSocket();
    ~UDPSocket();

    //“Conecta” este socket a um peer (armazena IP e porta internamente)
    //Internamente cria o socket UDP e configura timeout de 100 ms para recv
    //Retorna true em sucesso, false em caso de erro
    bool connectTo(const string& ip, uint16_t port);

    //Envia um serializado via UDP (sendto)
    //Retorna número de bytes enviados, ou –1 em caso de erro
    ssize_t send(const vector<char> segment);

    //Tenta receber um Segmento
    //Se não chegar nada em 100 ms ou ocorrer erro, retorna null.
    vector<char> receive();

    //Se alguma chamada falhar, getLastError() devolve o errno da última operação.
    int getLastError() const;

private:
    int                sockfd_;      // descritor de socket UDP
    struct sockaddr_in peerAddr_;    // endereço do peer (IP + porta)
    bool               isConnected_; // true se connectTo() já foi bem sucedido
    int                lastErrno_;   // armazena errno em erro
};

#endif 
