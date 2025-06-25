#include "package.hpp"

// Internal constants for package's size control
#define MAX_PACK_DATA 1440

void printBits(uint32_t value, int bitQntd) {
    //printf("bits:\n");
    for(int i = bitQntd - 1; i>= 0; i--) {
        if ((1<<i)&value) {
            printf("1");
        } else {
            printf("0");
        }
    }
    printf("\n");
    
}

void Package::printBufferBits(const vector<char>& buffer) {
    for (char c : buffer) {
        uint32_t val = (uint32_t) c;
        printBits(val, 8);
    }
    printf("\n");
}

//================================
// MÉTODOS PRINCIPAIS
//================================


// Para cada byte de um dado, escreve deus bits em little endian
void serializeField(vector<char> &buffer, uint32_t value, int byteQntd) {
    for(int i = 0; i < byteQntd; i++) {
        char opa = ((value & (0xFF << (byteQntd-i-1)*8)) >> (byteQntd-i-1)*8);
        printf("---");
        printBits(opa, 8);
        buffer.push_back(opa);
    }
    printf("\n");

}

// Transforma os dados do objeto na sequnecia de bits a ser trans
// mitida os enviando em um vector de chars;
vector<char> Package::serialize() const {
    vector<char> buffer;

    // Verificando se a quantidade de dados não ultrapassa o esperado
    if(this->data.size() > MAX_PACK_DATA)
        return {};

    // Convertendo os dados do cabeçalho para little endian
    buffer.insert(buffer.begin(), this->sid.begin(), this->sid.end());

    uint32_t combinedField2 = sttl << 5;
    combinedField2 |= ((uint32_t) this->flagC) << 4;
    combinedField2 |= ((uint32_t) this->flagR) << 3;
    combinedField2 |= ((uint32_t) this->flagACK) << 2;
    combinedField2 |= ((uint32_t) this->flagAR) << 1;
    combinedField2 |= ((uint32_t) this->flagMB) << 0;

    uint32_t combinedField = \
        // Write the STTL in the top 27 bits
        // Write the flags in the bottom 5 bits
        (static_cast<uint32_t>(this->flagC) << 31)
        | (static_cast<uint32_t>(this->flagR) << 30)
        | (static_cast<uint32_t>(this->flagACK) << 29)
        | (static_cast<uint32_t>(this->flagAR) << 28)
        | (static_cast<uint32_t>(this->flagMB) << 27)
        | this->sttl;

    printBits(combinedField2, 32);
        
    serializeField(buffer, combinedField, 4);
    serializeField(buffer, this->seqnum, 4);
    serializeField(buffer, this->acknum, 4);
    serializeField(buffer, this->window, 2);
    serializeField(buffer, this->fid, 1);
    serializeField(buffer, this->fo, 1);
        
    // Serializando os dados (basta copiar para o buffer)
    buffer.insert(buffer.end(), this->data.begin(), this->data.end());

    return buffer;
}

// reescreve os inteiros do vetor de bytes da resposta
uint32_t deserializeField(const vector<char> &buffer, int byteQntd, size_t &offset) {
    uint32_t val = 0;

    for(int i = 0; i < byteQntd; i++) {
        val <<= 8;
        val |= buffer[offset+(byteQntd - 1 - i)];
    }

    offset += byteQntd;
    return val;
}

Package Package::deserialize(const vector<char>& buffer) {
    Package pack;
    size_t offset = 0; // Offset para controlar o indice do próximo campo no pacote
    // 
    // Para o sid
    for (int i = 0; i < 16; i++) {
        pack.sid[i] = buffer[offset];
        offset++;
    }

    // Deserializando sttl e flags
    uint32_t combinedField = deserializeField(buffer, 4, offset);

    /*
    printf("Aquiiiiii\n");
    for(int i = 31; i>= 0; i--) {
        if ((1<<i)&combinedField) {
            printf("1");
        } else {
            printf("0");
        }
    }
    printf("\n\n");
    */

    // Get top 27 bits for the STTL
    pack.sttl = 0;
    for(int i = 0; i < 27; i++) {
        pack.sttl |= (1<<i)&combinedField;
    }

    // Reobtendo as flags
    pack.flagC = combinedField & (1 << 31);
    pack.flagR = combinedField & (1 << 30);
    pack.flagACK = combinedField & (1 << 29);
    pack.flagAR = combinedField & (1 << 28);
    pack.flagMB = combinedField & (1 << 27);


    // Deserializando o restante dos campos
    pack.seqnum = deserializeField(buffer, 4, offset);
    pack.acknum = deserializeField(buffer, 4, offset);
    pack.window = deserializeField(buffer, 2, offset);
    pack.fid = deserializeField(buffer, 1, offset);
    pack.fo = deserializeField(buffer, 1, offset);

    // Deserializando o payload de dados
    pack.data.reserve(buffer.size() - offset);
    pack.data.assign(buffer.begin() + offset, buffer.end());
    return pack;
}


//================================
// UTILITÁRIOS PARA FLAGS
//================================

// Pacotes ack devem ter números de ack iguais aos de seq, e não trasnportarem dados
// Além da flag ativa
bool Package::isAckOnly() const {
    return this->flagACK &&  (this->acknum == this->seqnum) && this->data.empty();
}

// Pacotes connect devem ter seqnum, acknum e sttl zerados;
bool Package::isConnect() const {
    // Verificando sttl e flags
    if(!(this->sttl == 0) || !(this->flagC))
        return false;
    
    // Números de sequencia e ack devem ser 0
    if((this->seqnum != 0) || (this->acknum) != 0)
        return false;

    // O pacote não pode ter dados
    if(!this->data.empty())
        return false;

    // Verificando se todo o SID possui 0
    for(char c : this->sid) {
        if(c != 0) return false;
    }

    return true;
}

// Pacotes disconnect devem ter as flags ack, conect e revive ligadas, além de
// não enviarem dados
bool Package::isDisconnect() const {
    return (this->flagACK) && (this->flagC) && (this->flagR) && this->data.empty();
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
array<char,16> Package::getSid() const {
    return this->sid;
}

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
void Package::setSid(array<char, 16> sid) {
    this->sid = sid;
}

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
    
    ss << "=== Pacote SLOW ===" << endl;

    // Informações do cabeçalho
    ss << "sid = ";
    for(char c : this->sid) {
        ss << c;
    }
    ss << endl;
    ss << "seq = " << this->seqnum << endl;
    ss << "ack = " << this->acknum << endl;
    ss << "Window = " << this->window << endl;

    // Caso o pacote seja de um segmento fragmentado
    if(this->flagMB) {
        ss << "fid = " << this->fid << endl;
        ss << "fo = " << this->fo << endl;
    }

    // Indentificando o tipo do pacote
    if(this->isAckOnly())
        ss << "[ACK PACKAGE]" << endl;
    if(this->isConnect())
        ss << "[CONNECT PACKAGE]" << endl;
    if(this->flagR)
    if(this->isDisconnect())
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
    ss << endl;

    return ss.str();
}

void Package::printAll() const {
    cout << "=== Pacote ===" << endl;

    // UUID
    cout << "sid: ";
    if(!this->sid.empty()) {
        for(char c : this->sid) {
            cout << c;
        }
    }
    cout << endl;

    cout << "sttl:" << this->sttl << endl;
    // Flags
    cout << "Flag c: " << this->flagC << endl;
    cout << "Flag r: " << this->flagR << endl;
    cout << "flag ack: " << this->flagACK << endl;
    cout << "flag ar: " << this->flagAR << endl;
    cout << "flag mb: " << this->flagMB << endl; 

    cout << "seqnum: " << this->seqnum << endl;
    cout << "acknum: " << this->acknum << endl;
    cout << "window: " << this->window << endl;
    cout << "fid: " << this->fid << endl;
    cout << "fo: " << this->fo << endl;

    if(!this->data.empty()) {
        for(char c : this->data) {
            cout << c;
        }
    }
    cout << endl;
}

// Para testar o buffer serializado

void Package::testPackage() {
    Package pgk;
    vector<char> testData = {'t', 'e', 's', 't', 'e'};

    // Pacote sem nada
    pgk.toString();

    // ack package
    pgk.setFlagACK(true);
    pgk.toString();
    cout << pgk.toString();

    // connect Package 
    pgk.setFlagACK(false);
    pgk.setFlagC(true);
    cout << pgk.toString();

    // data package
    pgk.setAcknum(2);
    pgk.setSeqnum(4);
    pgk.setFlagACK(true);
    pgk.setData(testData);
    pgk.setWindow(2);
    cout <<pgk.toString();

    // Serializando
    vector<char> serTest = pgk.serialize();
    pgk.setFlagACK(true);
    pgk.setFlagC(true);

    printBufferBits(serTest);
    cout << "Tamanho serialize:" << serTest.size() << endl;
    

    // Deserializando
    Package pgk2 = pgk.deserialize(serTest);
    cout << pgk2.toString();

    return;
}