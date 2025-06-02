#ifndef CONNECTION_H
#define CONNECTION_H

#include <cstdint>
#include "package.hpp"

class Connection
{
public:
    enum class State { CLOSED, CONNECT_SENT, ESTABLISHED, DISCONNECTED, REVIVE_SENT };

    // devolve o estado atual
    State getState() const;
    // retorna true se a conexão foi estabelecida
    bool isEstablished() const;

    // Atualiza o estado a partir de um pacote recebido
    void handleIncoming(const Package& package);

    // Preenche um pacote com os dados atuais da conexão (seq, ack, window, etc)
    void handleOutput(Package& package);

private:
    State state = State::CLOSED;

    uint32_t seqnum = 0;
    uint32_t acknum = 0;
    uint32_t sttl = 0;
    uint16_t window = 0;
};

#endif
