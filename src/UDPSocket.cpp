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

    

}