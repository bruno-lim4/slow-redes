#include "package.hpp"

// Internal constants for package's size control
#define MAX_PACK_DATA 1440

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

// Transforma os dados do objeto na sequnecia de bits a ser trans
// mitida os enviando em um vector de chars;
vector<char> Package::serialize() const {
    vector<char> serializedBuffer;

    // Verificando se a quantidade de dados não ultrapassa o esperado
    if(this->data.size() > MAX_PACK_DATA)
        return {};

    // 

}
static Package Package::deserialize(const vector<char>& buffer);


//================================
// UTILITÁRIOS PARA FLAGS
//================================

// Pacotes ack devem ter números de ack iguais aos de seq, e não trasnportarem dados
// Além da flag ativa
bool Package::isAckOnly() const {
    return this->flagACK && (this->acknum == this->seqnum) && this->data.empty();
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
uint32_t Package::getSttl() const {
    return this->sttl;
}

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
    
    // Informações do cabeçalho
    // ss << "sid = " << this->sid << endl;
    ss << "seq = " << this->seqnum << endl;
    ss << "ack = " << this->acknum << endl;
    ss << "Window" << this->window << endl;

    // Caso o pacote seja de um segmento fragmentado
    if(this->flagMB) {
        ss << "fid = " << this->fid << endl;
        ss << "fo = " << this->fo << endl;
    }

    // Indentificando o tipo do pacote
    if(this->isAckOnly())
        ss << "[ACK PACKAGE]" << endl;
    if(this->flagC)
        ss << "[CONNECT PACKAGE]" << endl;
    if(this->flagR)
        ss << "[REVIVE PACKAGE]" << endl;
    if(this->flagAR)
        ss << "[ACCEPTED]" << endl;
    
    // Imprimindo os dados se houver
    if(!this->data.empty()) {
        ss << "DATA: ";
        for(char c : this->data) {
            ss << c;
        }
    }

    return ss.str();
}