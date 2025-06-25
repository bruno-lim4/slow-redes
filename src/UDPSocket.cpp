#include "UDPSocket.hpp"
#include "package.hpp"

/*construtor do socket 
indica que o socket não foi criado ainda
que não há conexão e que não houve erro resgistrado por enquanto 
*/

UDPSocket::UDPSocket(){
    sockfd_ = -1;  
    isConnected_= false ;
    lastErrno_ = 0;
}

/*destrutor 
encerra o socket caso ainda esteja aberto 
reseta todos os estados, indicando que não há mais conexão 
que não há erros registrados e que o socket não existe mais 
*/
UDPSocket::~UDPSocket(){
    if (sockfd_ >= 0) {
        ::close(sockfd_);
        sockfd_ = -1;
        isConnected_ = false ; 
        lastErrno_ = 0 ; 
    }
}

/*
cria um socket, registra 
o IP e porta destino (em peerAddr_), 
usa inet_pton para validar o IP 
se tudo foi bem-sucedido teremos isConnected_ como true 
se algum erro ocorreu a função retorna falso 
*/

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
        cout << "Erro\n";
        return false;
    }

    //socket criado corretamente 
    isConnected_ = 1 ; 
    return true ;

}

/*
verifica se o socket foi previamente conectado 
envia todos os dados para o destino (usando sendto) 
retorna o número de bytes enviados ou -1 em caso 
de erro (atualizando lastErrno_ nesse caso) 
*/

ssize_t UDPSocket::send(const vector<char> &segment){

    //verifica se connectTo foi chamado com sucesso 
    if(!isConnected_){
        lastErrno_ = ENOTCONN ; 
        return -1 ; 
    }
    
    //mandar os bytes via UDP para o endereço IP e porta armazenados em peerAddr_
    ssize_t sent = sendto(
        sockfd_, 
        segment.data(), 
        segment.size(), 
        0, 
        reinterpret_cast<const struct sockaddr*>(&peerAddr_), 
        sizeof(peerAddr_)) ;

    //checando se houve erro no envio
    if(sent < 0){
        lastErrno_  = errno ; 
        return -1 ; 
    }

    printf("Enviando:");
    Package::printBufferBits(segment);
    printf("\n");

    return sent ; 
}

/*
aguarda um tempo limite por dados, se der timeout ou erro retorna nullopt
se há dados para receber a função recvfrom() é utilizado, 
guardando os dados recebidos em vector<char> 
*/
optional<vector<char>> UDPSocket::receive(uint32_t timeout){
    if(sockfd_ < 0){ //socket não foi bem estabelecido
        lastErrno_ = EBADF ; 
        return nullopt ; 
    }

    //descritores para leitura
    fd_set readfd;
    FD_ZERO(&readfd);
    FD_SET(sockfd_, &readfd);

    struct timeval tv ; 
    tv.tv_sec = timeout/1000 ; //quantos segundos inteiros 
    tv.tv_usec = timeout ; 

    //bloquear até ter dados para leitura ou estourar o tv (time limit)
    int sel = select(sockfd_ + 1, &readfd, nullptr, nullptr, &tv);


    if(sel < 0){
        lastErrno_ = errno ; 
        return nullopt ; 
    }

    //não recebeu nada (estourou tempo limite)
    if(sel == 0) return nullopt ; 


    //sel > 0 -> sockfd_ está pronto pra leitura 
    char buffer[TAM_BUF];

    //ler o que foi enviado e salvar no buffer 
    ssize_t recvd = ::recvfrom(
        sockfd_,
        buffer,
        sizeof(buffer),
        0,
        nullptr,
        nullptr
    );

    //deu erro de leitura
    if(recvd < 0){
        lastErrno_ = errno ; 
        return nullopt ; 
    }

    vector<char> result(static_cast<size_t>(recvd));
    memcpy(result.data(), buffer, static_cast<size_t>(recvd));
    
    printf("Recebendo:\n");
    Package::printBufferBits(result);
    printf("\n");

    return result;

}

// função apenas para retornar o último erro 
int UDPSocket::getLastError() const{
    cout << "sou o socket";
    return lastErrno_ ; 
}
