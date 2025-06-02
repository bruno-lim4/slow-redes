#include "UDPSocket.h"

#include <sys/socket.h>  
#include <arpa/inet.h>    
#include <netinet/in.h>   
#include <unistd.h>       
#include <cstring>        
#include <cerrno>         

UDPSocket::UDPSocket():   
{
    sockfd_ = -1;  
    isConnected_= false
    lastErrno_ = 0;
}

UDPSocket::~UDPSocket() {
    if (sockfd_ >= 0) {
        ::close(sockfd_);
        sockfd_ = -1;
        isConnected_ = false ; 
        lastErrno_ = 0 ; 
    }
}

bool UDPSocket::connectTo(const string& ip, uint16_t port) {

    //criar a conexão - nesse caso gostariamos de utilizar 
    //socket UDP, o protocolo usado será o mesmo portanto não precisamos especificar este campo
    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0) ;

    if(sockfd_ < 0){//deu erro 
        lastErrno_ = errno ; 
        return false ; 
    }

    memset(&peerAddr_, 0, sizeof(peerAddr_));
    peerAddr_.sin_family = AF_INET ; // marcar que estamos usando IPv4 
    peerAddr_.sin_port = htons(port);

    //estabelecendo (IP, porta) -> o IP ficará armazenado em .sin_addr
    int rc = inet_pton(AF_INET, ip.c_str(), &peerAddr_.sin_addr);

    //É um IPv4 válido? 
    if (rc <= 0) {
        // rc == 0 -> ip string inválida; rc < 0 -> erro 
        lastErrno_ = (rc == 0 ? EINVAL : errno);
        ::close(sockfd_);
        sockfd_ = -1;
        return false;
    }

    //socket criado corretamente 
    isConnected_ = 1 ; 
    return true ;

}

ssize_t send(const vector<char> segment){

    //verifica se connectTo foi chamado com sucesso 
    if(!isConnected_){
        lastErrno_ = ENOTCONN ; 
        return -1 ; 
    }

    //mandar os bytes via UDP para o endereço IP e porta armazenados em peerAddr_
    ssize_t :: sent = sendto(
        sockfd_, 
        segment, 
        segment.size(), 
        0, 
        reinterpret_cast<const struct sockaddr*>(&peerAddr_), 
        sizeof(peerAddr_)) ;

    //checando se houve erro no envio
    if(sent < 0){
        lastErrno_  = errno ; 
        return -1 ; 
    }

    return sent ; 
}