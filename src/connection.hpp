#ifndef CONNECTION_H
#define CONNECTION_H

#include <cstdint>
#include "package.hpp"

class Connection
{
public:
    enum class State { CLOSED, CONNECT_SENT, ESTABLISHED, DISCONNECT_SENT, DISCONNECTED, REVIVE_SENT};

    // devolve o estado atual
    State getState() const;
    // retorna true se a conexão foi estabelecida
    bool isEstablished() const;

    // Atualiza o estado a partir de um pacote recebido
    void handleIncoming(const Package& package);

    // Preenche um pacote com os dados atuais da conexão (seq, ack, window, etc)
    // opt pode ser:
    /*
        1 - Preenche um pacote de dados normal (se nao estiver conectado só ignora)
        2 - Preenche um connect (se ja tiver conectado só ignora)
        3 - Preenche um disconnect (se ja tiver desconectado so ignora)
        4 - Preenche um revive (se ja tiver conectado só ignora)
    */
    void handleOutput(Package& package, int opt);

private:
    State state = State::CLOSED;

    uint32_t seqnum = 0;
    uint32_t acknum = 0;
    uint32_t sttl = 0;
    // adiciona sid
};

#endif
