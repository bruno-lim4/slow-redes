#include "connection.hpp"

Connection::State Connection::getState() const {
    return this->state;
}

bool Connection::isEstablished() const {
    return (this->state == Connection::State::ESTABLISHED);
}

void Connection::handleIncoming(const Package& package) {
    switch (this->state)
    {
    case State::CLOSED:
        // nao faz sentido fazer nada aq
        break;
    
    case State::CONNECT_SENT:
        // vejo se recebi o setup aq
        break;
    
    case State::ESTABLISHED:
        // espero um ack doq foi enviado
        break;
    
    case State::DISCONNECTED:
        // espero ack
        break;
    
    case State::REVIVE_SENT:
        // ack mas com accepted/failed ligada
        break;

    default:
        break;
    }
} 