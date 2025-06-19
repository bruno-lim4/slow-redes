# slow-redes

## Grupo

* Pietra Gullo Salgado Chaves - 14603822 

* Bruno Figueiredo Lima - 14562383

* Gabriel Antunes Afonso de Araujo - 14571077

## Arquitetura proposta:

Todos os atributos são privados com getters e setters quando necessário

### **Package:**

Deve definir, instanciar e fornecer os pacotes do protocolo SLOW.

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

#### Métodos:

- Serialize

- Deserialize

- Methodos para checar se cada flag está ativa

- toString para debug

### **protocolHandler:**

Integrar, configura e controlar o fluxo da comunicação e os segentos mandados.

ProtocolHander fica a cargo do pacote, tirando campos como seqnum e acknum, e flags que dependem do estado da comunicação no momento, ficando esses a cargo de Connection

#### Atributos:

| Nome | Função | Quantidade de bits |
|------|--------|--------------------|
|UDPSocket|||
|Connection|||

#### Métodos:

- initiateHandshake(UDPSocket): Recebe os parametros para criar a conexão, faz o 3 way connect

    * Vai se comunicar com o Connection para obter, estados acks e seqs
    * Instancia os segmentos a partir disso
    * Vai enviar os pacotes do handshake pelo UDPSocket

- SendData(Dados): Recebe os dados a serem enviados pelo usuário, verifica se existe uma conexão e se está ativa, separa os dados em fragmentos (quando necessário) envia e espera o ack

    * Usa receiveLoop() para esperar os acks
    * Consulta o connection para criar os pacotes / segmentos
    * Altera / instancia objetos Packages

- recieveLoop(): Usado internamente para esperar os acks e as respostas do servidor durante as conexões (3 way ou 0 way)

### **Connection:**

Máquina de estados, para gerenciar acks, seqs atuais e o estado da comunicação (ou seja, qual tipo de segmento deve ser enviado)

#### Atributos:

enum para os states (CONNECT_SENT, CLOSED, ESTABLISHED, DISCONNECTED, REVIVE_SENT)

sid e sttl variáveis internas comuns a conexão

#### Métodos:

- handleIncoming(): Recebe um pacote da protocolHandler (após o receiveLoop), verificando suas flags e valores para atualizar a máquina de estado e variáveis internas

- handleOutput(): Recebe uma referencia para package para ser alterado de acordo com o estado da conexão, setando falgs e sid.

    * ProtocolHandler chama essa função antes de enviar os pacotes SLOW

- AdvanceAck() e advanceSeq(): Funções internas para controlar a seqnum e acknum

- Getters e setters para o que for preciso


