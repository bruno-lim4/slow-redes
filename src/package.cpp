#include "package.hpp"

// Internal constants for package's size control
#define MAX_PACK_DATA 1440


void printBits(char byte) {
    for (int i = 7; i >= 0; --i) {
        std::cout << ((byte >> i) & 1);
    }
    std::cout << std::endl;
}

void printBufferBits(const std::vector<char>& buffer) {
    for (unsigned char c : buffer) {
        printBits(c);
    }
    printf("\n");
}

//================================
// MÉTODOS PRINCIPAIS
//================================

// Função auxiliar que serializa campos genéricos em um buffer
// Os bites são escritos em little endian
template<typename T>
void serializeField(vector<char> &buffer, T value) {
    using Unsigned = std::make_unsigned_t<T>;
    Unsigned v = static_cast<Unsigned>(value);

    for (std::size_t i = 0; i < sizeof(T); ++i) {
        buffer.push_back(static_cast<char>( (v >> (i * 8)) & 0xFF ));
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
    cout << combinedField << endl;
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
    using Unsigned = std::make_unsigned_t<T>;
    Unsigned value = 0;

    // Deserializando cada byte
    for (std::size_t i = 0; i < sizeof(T); ++i) {
        value |= static_cast<Unsigned>(
                     static_cast<unsigned char>(buffer[offset + i])
                 ) << (i * 8);
    }

    // Avançando o ponteiro para a leitura
    offset += sizeof(T);       
    return static_cast<T>(value);

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
    printf("CombinedField: %d\n", combinedField);

    for(int i = 0; i < 32; i++) {
        if((1 << (31 - i)) & combinedField)
            cout << 1;
        else
            cout << 0;
    }

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
    return this->flagACK &&  (this->acknum == this->seqnum) && this->data.empty();
}

// Pacotes connect devem ter seqnum, acknum e sttl zerados;
bool Package::isConnect() const {
    return (this->sttl == 0) && (this->flagC)
    && (this->seqnum == 0) && (this->acknum == 0)
    && (this->data.empty());
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

bool Package::setData(vector<char> data) {
    if(data.size() > MAX_PACK_DATA)
        return false;
    
    this->data = data;
    return true;
}


//================================
// DEBUGS E I/O
//================================
string Package::toString() const {
    stringstream ss;
    
    ss << "=== Pacote SLOW ===" << endl;

    // Informações do cabeçalho
    // ss << "sid = " << this->sid << endl;
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
        for(char c : this->uuid) {
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
    pgk.setFlagACK(true);
    pgk.setData(testData);
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