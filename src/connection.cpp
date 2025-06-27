#include "connection.hpp"

Connection::State Connection::getState() const {
    return this->state;
}

bool Connection::isEstablished() const {
    return (this->state == State::ESTABLISHED);
}

void Connection::handleIncoming(const Package& package) {
    switch (this->state)
    {
    case State::CONNECT_SENT:
        // se nao recebeu o setup, descarta
        if (!package.isAccept()) return;

        this->sttl = package.getSttl();
        this->sid = package.getSid();
        this->seqnum = package.getSeqnum()+1;
        this->acknum = package.getSeqnum();

        this->state = State::ESTABLISHED;

        break;
    
    case State::ESTABLISHED:
        // espero um ack doq foi enviado
        if (!package.isAckOnly()) return;

        if (package.getAcknum() == this->seqnum) {
            this->seqnum++;
            this->acknum = package.getSeqnum();

            this->sttl = package.getSttl();
            
            this->state = State::ESTABLISHED;
        }
        break;
    
    case State::DISCONNECT_SENT:
        // espero ack
        if (!package.isAckOnly()) return;

        if (package.getAcknum() == this->seqnum) {
            this->seqnum++;
            this->acknum = package.getSeqnum();

            this->sttl = package.getSttl();
            
            this->state = State::DISCONNECTED;
        }

        break;
    
    case State::REVIVE_SENT:
        // ack mas com accepted/failed ligada
        if (package.isAckOnly() && package.isAccept()) {
            // deu bom, checa se dá ack no passado (minha req de revive)
            if (package.getAcknum() == this->seqnum) {
                this->seqnum++;
                this->acknum = package.getSeqnum();

                this->sttl = package.getSttl();

                this->state = State::ESTABLISHED;
            }
            
        } else if (!package.isAccept()) {
            // foi rejeitado
            cout << "conexão rejeitada\n";
            this->state = State::DISCONNECTED;
        }

        break;

    default:
        break;
    }
} 

void Connection::handleOutput(Package& package, int opt) {
    switch (opt)
    {
    case 1:
        // manda dados normal
        if (!(this->state == State::ESTABLISHED)) break;
        package.setFlagACK(true);
        package.setAcknum(this->acknum);
        package.setSeqnum(this->seqnum);
        package.setSid(this->sid);

        this->state = State::ESTABLISHED;
        break;
    
    case 2:
        // manda connect
        if (!(this->state == State::CLOSED)) break;
        package.setFlagC(true);
        package.setSeqnum(0);
        package.setAcknum(0);

        this->state = State::CONNECT_SENT;
        break;
    
    case 3:
        // manda um disconnect
        if (!(this->state == State::ESTABLISHED)) break;
        package.setFlagACK(true);
        package.setFlagC(true);
        package.setFlagR(true);
        package.setAcknum(this->acknum);
        package.setSeqnum(this->seqnum);
        package.setSid(this->sid);

        this->state = State::CONNECT_SENT;
        break;
    
    case 4:
        // manda um revive 
        if (!(this->state == State::DISCONNECTED)) break;
        package.setFlagACK(true);
        package.setFlagR(true);
        package.setAcknum(this->acknum);
        package.setSeqnum(this->seqnum);
        package.setSid(this->sid);

        this->state = State::REVIVE_SENT;
    
    default:
        break;
    }
}