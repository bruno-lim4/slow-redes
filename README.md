# slow-redes

## Grupo

* Pietra Gullo Salgado Chaves - 14603822 

* Bruno Figueiredo Lima - 14562383

* Gabriel Antunes Afonso de Araujo - 14571077

## Arquitetura proposta:

Arquitetura em camadas, onde todos os atributos são privados com getters e setters quando necessário.

### **Package:**

Define, instanciar e fornece os pacotes do protocolo SLOW.

#### Atributos:

| Nome | Função | Quantidade de bits |
|------|--------|--------------------|
| sid | UUID definido como um array de chars | 128 bits |
| sttl | Time to live | 27 bits |
| c | Flag que indica o pacote Connect | 1 bit |
| r | Flag que indica o pacote Revive | 1 bit |
| ack | Flag que indica o pacote de ack | 1 bit |
| a/r | Falg que incia se o pacote aceita ou rejeita uma conexão | 1 bit |
| mb | Indica que é um pacote de segmento fragmentado | 1 bit |
| seqnum | Número único do segmento de uma seção | 32 bits |
| acknum | Sequence number do último pacote recebido | 32 bits |
| window | Window size | 16 bitw |
| fid | indentificador do pacote fragmentado | 8 bits |
| fo | Ordem dos pacotes de um mesmo fid | 8 bits
| data | dados da mensage | até 1440 bytes

#### Métodos principais:

- Serialize: Separa os bytes da classe e os armazena em um vector de chars.

- Deserialize: Dado um vector de chars com os dados serializados do pacote, resonstroi o objeto original.

- Methodos para checar se cada flag está ativa.

- toString para debug.

### **protocolHandler:**

Integra, configura e controla o fluxo da comunicação e os segentos mandados, implementando métodos tratar cada tipo de fluxo de mensagens do protocólo.

#### Atributos:

| Nome | Função | Tipo |
|------|--------|------|
|fid|Armazena o fid local de uma comunicação|uint8_t|
|Connection|Objeto que gerencia localmente os estados da comunicação|Connection|

#### Métodos principais:

- handshake: Recebe os parametros para criar a conexão e então faz o 3-way connect.

- SendData: Envia dados do peripheral para o central, desde que haja uma conexão ativa. Além disso, *fragmenta* o pacote sempre que necessário.

- sendDisconect: Desabilita a conexão préviamente estabelecida com o central. Para isso envia o pacote "disconect" e verifica no ack se a conexão realmente não está mais ativa.

- sendRevive: Dada uma conexão que já havia existido, mas foi desabilidata com "sendDisconnect", reastabelece implementando o processo do 0-way connect.

- recieveLoop: Usado internamente para esperar os acks e as respostas do servidor durante as conexões (3-way ou 0-way).

### **Connection:**

Máquina de estados, que garante que os campos sid e sttl sejam preenchidos, gerencia os núemros de ack e seq atuais, bem como o estado da comunicação (estabelecida, disconectada, fechada, ect...) e implementa funções para setar o header do pacote de cada estado, bem como transitar entre eles conforme a comunicação avança.

#### Atributos:

|nome|função|tipo|
|state|Armazena o estado atual da comunicação|State|
|acknum|Armazena o último acknum enviado pelo central|uint32_t|
|seqnum|Armazena o último seqnum enviado|uint32_t|
|sttl|armazena o sttl estabelecido pelo central|uint32_t|
|sid|armazena o sid estabelecido pelo central|array<char,16>|

É importante citar que utilizamos um enum para os states (CONNECT_SENT, CLOSED, ESTABLISHED, DISCONNECT_SENT, DISCONNECTED, REVIVE_SENT).

#### Métodos principais:

- handleIncoming(): Recebe um pacote do protocolHandler (após o receiveLoop), verificando suas flags e valores para atualizar a máquina de estado e variáveis internas.

- handleOutput(): Monta um pacote de acordo com os parametros atuais da conexão estabelecida (sid, sttl, acknum e seqnum) e os requisitos do tipo de pacote que o estado atual pede. 

- GetState: retorna o estado atual da conexão.

- isEstablished: Verifica se há uma conexão ativa com o central.


### **UDPSocket:**

Base da arquitetura de camadas, sendo a classe que encapsula as primitivas do socket UDP para cria-lo, destruí-lo, conectá-lo e enviar mensagens por ele de forma simplificada por quem a utiliza.

#### Atributos:

|nome|função|tipo|
|sockfd_|descritor de socket UDP|int|
|peerAddr_|Endereço do peer|sockaddr_in|
|isConnected_|Informa se o socket está conectado|bool|
|lastErrno_|Armazena o erro dos serv|int|

#### Métodos principais:

- connectTo: Conecta o udp socket ao central.

- disconnect: Desconecta e fecha o socket após o fim da comunicação.

- send: Envia dados serializados pelo socket, uma vez que esta está conectado.

- receive: Seta um timeout e espera o recebimento de um segmento após um envio.

## Como rodar?

Entre na pasta do projeto e compile o código com:

```bash
    cd src/
    make
```

Após a compilação, execute o programa com:

```bash
    ./peripheral
```

No incício da execução, uma interface apareçerá para mostrar cada comando.

Vale ressaltar que o envio de mensagens (comando 2) só ocorre após quebrar a linha duas vezes.

## Exemplos de uso

* Conectar e enviar uma mensagem:

```bash
    1
    2
    Uma nova mensagem sendo enviada

    
```

* Conectando, desconectando e enviando um revive

```bash
    1
    3
    4
```

* Enviando várias mensagens e encerrando o programa

  ```bash
    1
    2
    Primeira mensagem

    2
    Segunda mensagem

    3
    Mais uma mensagem

    5
  ```
