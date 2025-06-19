#ifndef PACKAGE_H
#define PACKAGE_H
    #include <iostream>
    #include <vector>
    #include <sstream>
    #include <cstdint>

    using namespace std;

    class Package {
    private:
        // campos
        //UUID sid;
        uint32_t sttl = 0;    // Time to live (convertido para 27 bits no pacote final)
        uint32_t seqnum = 0;  // Número de sequência
        uint32_t acknum = 0;  // Número de acknowledgement
        uint16_t window = 0;
        uint8_t fid = 0;      // Id para pacotes de um segmento fragmentado
        uint8_t fo = 0;       // Indica a ordem de pacotes fragmentados

        // Flags
        bool flagC = false;    // indica um pacote Connect
        bool flagR = false;    // indica um pacote Revive
        bool flagACK = false;  // indica um pacote ack
        bool flagAR = false;   // indica aceitação/rejeição da conexão
        bool flagMB = false;   // indica um pacote fragmentado

        // Dado
        vector<char> data;

    public:

        // Métodos principais
        vector<char> serialize() const;
        static Package deserialize(const vector<char>& buffer);

        // Métodos utilitários para flags
        bool isAckOnly() const;
        bool isConnect() const;
        bool isDisconnect() const;
        bool isRevive() const;
        bool isAccept() const;
        bool isMoreBits() const;

        // Getters
        uint32_t getSttl() const;
        vector<char> getData() const;
        uint32_t getAcknum() const;
        uint32_t getSeqnum() const;
        uint8_t getFid() const;
        uint8_t getFo() const;

        // Setters
        // void setSid(UUID newSid);
        void setSttl(uint32_t newSttl);
        void setSeqnum(uint32_t newSeqNum);
        void setAcknum(uint32_t newAckNum);
        void setWindow(uint16_t newWindow);
        void setFid(uint8_t newFid);
        void setFo(uint8_t newFo);
        void setFlagC(bool newFlag);
        void setFlagR(bool newFlag);    
        void setFlagACK(bool newFlag);
        void setFlagAR(bool newFlag);
        void setFlageMB(bool newFlag);
        void setData(vector<char> data);
        

        // Métodos para debug/log (opcional)
        string toString() const;
        void printAll() const;
        static void testPackage();
    };

#endif

/*
ANOTAÇÕES DO GABIRUUU PARA UM FUTURO PRÓXIMO
metodos static não estão atrelados ao objeto em si, mas sim a classe... Usar com
nome_da_clae::metodo  (só achei legal usar isso aqui :) )
*/