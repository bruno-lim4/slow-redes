#include "package.hpp"

// Construtor da classe
Package::Package(
        uint32_t sttl,
        bool connect,
        bool revive,
        bool ack,
        bool accept,
        bool more_bits,
        uint32_t seq,
        uint32_t ackn,
        uint16_t win,
        uint8_t frag_id,
        uint8_t frag_off,
        const std::vector<char>& payload)
    : sttl(sttl),
      flagC(connect),
      flagR(revive),
      flagACK(ack),
      flagAR(accept),
      flagMB(more_bits),
      seqnum(seq),
      acknum(ackn),
      window(win),
      fid(frag_id),
      fo(frag_off),
      data(payload)
{}


//================================
// MÉTODOS PRINCIPAIS
//================================
vector<char> Package::serialize() const;
static Package Package::deserialize(const vector<char>& buffer);


//================================
// UTILITÁRIOS PARA FLAGS
//================================

// Pacotes ack devem ter números de ack iguais aos de seq, e não trasnportarem dados
// Além da flag ativa
bool Package::isAckOnly() const {
    return this->flagACK && (this->acknum == this->seqnum) && data.empty();
}

bool Package::isConnect() const {
    return this->flagC;
}

bool Package::isRevive() const {
    return this->flagR;
}

bool Package::isAccept() const {
    return this->flagAR;
}

bool Package::isMoreBits() const {
    return this->flagMB;
}


//================================
// GETTERS
//================================
vector<char> Package::getData() const {
    return this->data;
}

uint32_t Package::getAcknum() const {
    return this->acknum;
}

uint32_t Package::getSeqnum() const {
    return this->seqnum;
}

uint8_t Package::getFid() const {
    return this->fid;
}

uint8_t Package::getFo() const {
    return this->fo;
}


//================================
// SETTERS
//================================
void Package::setSeqnum(uint32_t newSeqNum) {
    this->seqnum = newSeqNum;
}

void Package::setAcknum(uint32_t newAckNum) {
    this->acknum = newAckNum;
}

void Package::setWindow(uint16_t newWindow) {
    this->window = newWindow;
}

void Package::setFid(uint8_t newFid) {
    this->fid = newFid;    
}

void Package::setFo(uint8_t newFo) {
    this->fo = newFo;
}

void Package::setFlagC(bool newFlag) {
    this->flagC = newFlag;
}

void Package::setFlagR(bool newFlag) {
    this->flagR = newFlag;
}   

void Package::setFlagACK(bool newFlag) {
    this->flagACK = newFlag;
}

void Package::setFlagAR(bool newFlag) {
    this->flagAR = newFlag;
}

void Package::setFlageMB(bool newFlag) {
    this->flagMB = newFlag; 
}

void Package::setData(vector<char> data) {
    this->data = data;
}


//================================
// DEBUGS E I/O
//================================
string Package::toString() const {
    stringstream ss;

    ss << "seq = " << seqnum << endl;
    ss << "ack = " << acknum << endl;
    if (isAckOnly())
}