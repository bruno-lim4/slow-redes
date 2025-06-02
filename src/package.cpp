#include "package.hpp"

// Internal constants for package's size control
#define MAX_PACK_DATA 1440

//================================
// MÉTODOS PRINCIPAIS
//================================

// Função auxiliar que serializa campos genéricos em um buffer
template<typename T>
void serializeField(vector<char> &buffer, T value) {
    for(size_t i = 0; i < sizeof(T); i += 8) {
        buffer.push_back((value >> i) & 0xFF);
    }
}

// Transforma os dados do objeto na sequnecia de bits a ser trans
// mitida os enviando em um vector de chars;
vector<char> Package::serialize() const {
    vector<char> buffer;

    // Verificando se a quantidade de dados não ultrapassa o esperado
    if(this->data.size() > MAX_PACK_DATA)
        return {};

    // Convertendo os dados do cabeçalho para little endian
    //serializeField(buffer, this->sid);

    // Juntando sstl com as flags do pacote na serialização
    uint32_t combinedField = (this->sttl & 0x07FFFFFF)
        | ((this->flagC ? 1 : 0) << 27)
        | ((this->flagR ? 1 : 0) << 28)
        | ((this->flagACK ? 1 : 0) << 29)
        | ((this->flagAR ? 1 : 0) << 30)
        | ((this->flagMB ? 1 : 0) << 31);
    serializeField(buffer, combinedField);

    serializeField(buffer,  this->seqnum);
    serializeField(buffer, this->acknum);
    serializeField(buffer, this->window);
    serializeField(buffer, this->fid);
    serializeField(buffer, this->fo);

    // Serializando os dados (basta copiar para o buffer)
    buffer.insert(buffer.end(), this->data.begin(), this->data.end());

    return buffer;
}

// Não sei se isso vai funcionar :(
template<typename T>
T deserializeField(const vector<char> &buffer, size_t &offset) {
    T value = 0;
    for(size_t i = 0; i < sizeof(T); i++) {
        value |= (static_cast<uint8_t>(buffer[offset + i] << (i * 8)));
    }
    offset += sizeof(T);

    return value;
}

Package Package::deserialize(const vector<char>& buffer) {
    Package pack;
    size_t offset = 0; // Offset para controlar o indice do próximo campo no pacote

    /*
    para o sid
    for (int i = 0; i < 16; ++i) {
        seg.sid[i] = static_cast<uint8_t>(buffer[offset++]);
    }
    */

    // Deserializando sttl e flags
    uint32_t combinedField = deserializeField<uint32_t>(buffer, offset);
    pack.sttl = combinedField & 0x07FFFFFF;
    pack.flagC = combinedField & (1 << 27);
    pack.flagR = combinedField & (1 << 28);
    pack.flagACK = combinedField & (1 << 29);
    pack.flagAR = combinedField & (1 << 30);
    pack.flagMB = combinedField & (1 << 31);

    // Deserializando o restante dos campos
    pack.seqnum = deserializeField<uint32_t>(buffer, offset);
    pack.acknum = deserializeField<uint32_t>(buffer, offset);
    pack.window = deserializeField<uint16_t>(buffer, offset);
    pack.fid = deserializeField<uint8_t>(buffer, offset);
    pack.fo = deserializeField<uint8_t>(buffer, offset);

    // Deserializando o payload de dados
    pack.data.assign(buffer.begin() + offset, buffer.end());
    return pack;
}


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
void Package::setSttl(uint32_t newSttl) {
    this->sttl = newSttl;
}

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