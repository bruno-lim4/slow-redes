#include <iostream>
#include <string>
#include "protocolHandler.hpp"
#include "UDPSocket.hpp"

// 142.93.184.175:7033

#define SERVER_IP "142.93.184.175"
#define SERVER_PORT 7033

int main() {
    //const string serverIp = "142.93.184.175";
    //uint16_t serverPort = 7033;

    UDPSocket socket;
    ProtocolHandler pc;
    bool checkCom;

    checkCom = pc.handshake(socket, SERVER_IP, SERVER_PORT);
    if(checkCom)
        cout << "Handshake bem sucedido" << endl;
    else
        cout << "Não foi possível estabelecer conexão" << endl;

    //string msg_transmitir = "Nos primordios da Internet, as redes de comunicacao evoluiram rapidamente, mas enfrentaram um desafio crucial: a necessidade de garantir transferencia de dados eficiente e confiavel mesmo em cenarios com alta latencia ou baixa largura de banda. Ja nos anos 1970 e 1980, pesquisadores como Vint Cerf e Bob Kahn trabalharam no desenvolvimento do TCP/IP (Transmission Control Protocol/Internet Protocol), que se tornaria o conjunto de protocolos dominante. Esse modelo introduziu conceitos fundamentais como fragmentacao de pacotes, roteamento dinamico e controle de congestionamento, permitindo que diferentes redes fisicas se comunicassem de forma transparente e resiliente, mesmo quando percursos alternativos fossem necessarios em funcao de falhas em enlaces ou de nos temporariamente sobrecarregados. A partir dai, surgiram mecanismos de controle de fluxo - baseados em janelas deslizantes (sliding windows) - e de congestionamento, que utilizavam algoritmos como Tahoe, Reno, New Reno e, posteriormente, CUBIC e BBR. Cada um desses algoritmos procurou otimizar o uso de recursos disponiveis, regulando a taxa de envio de pacotes para evitar colapsos de rede e garantindo uma utilizacao equilibrada entre multiplos fluxos concorrentes. Em particular, o algoritmo BBR (Bottleneck Bandwidth and Round-trip propagation time) passou a adotar uma abordagem distinta: em vez de reagir a perdas de pacotes, ele mede diretamente a largura de banda do gargalo e o tempo de ida e volta, ajustando sua janela de envio para maximizar a taxa de transferencia sem induzir filas indesejadas em buffers de roteadores. Entretanto, mesmo com essas evolucoes, ambientes heterogeneos e de alta variabilidade - como redes sem fio, sistemas moveis e satelites - continuam a representar desafios significativos. As flutuacoes na qualidade do canal de radio, interferencias, handovers entre celulas e passagens entre diferentes operadores exigem que protocolos de transporte e de enlace apresentem niveis adicionais de robustez, adaptabilidade e compensacao de erros. Por isso, desenvolvimentos recentes passaram a explorar tecnicas de codificacao de rede (network coding), em que os proprios roteadores podem recombinar pacotes de forma a aumentar a eficiencia da retransmissao, e de multipath TCP (MPTCP), que permite a um unico fluxo TCP utilizar multiplos caminhos simultaneos, aumentando a resiliencia e agregando capacidade de enlace. Nos contextos industriais e de data centers, outra fronteira de inovacao ganhou importancia: o uso de Quality of Service (QoS) para priorizacao de trafego critico. Em aplicacoes de voz sobre IP (VoIP), videoconferencia e controle de processos industriais, atrasos e variacoes de jitter podem comprometer seriamente a qualidade percebida pelo usuario. Assim, tecnicas como filas diferenciadas (DiffServ), marcacao de pacotes e escalonamento por prioridades (priority queuing) permitem que pacotes de maior importancia recebam tratamento preferencial nos roteadores, minimizando perdas e atrasos. Paralelamente, o advento de redes definidas por software (SDN - Software Defined Networking) e de arquiteturas baseadas em redes virtuais sobrepostas (overlay networks) abriu caminho para um controle mais flexivel e centralizado da topologia e das politicas de encaminhamento. Com SDN, controladores dedicados podem monitorar o estado da rede em tempo real e reconfigurar dinamicamente tabelas de fluxo nos switches, implementando balanceamento de carga inteligente, deteccao de falhas imediata e adaptacoes automaticas as condicoes de trafego. Nas overlays, tecnicas como tuneis criptografados (VPNs, GRE, VXLAN) permitem construir topologias logicas que atravessam diversas infraestruturas fisicas, garantindo isolamento de trafego e optimizacao de caminhos. Em paralelo a infraestrutura, camadas de aplicacao passaram a empregar protocolos especializados para contornar limitacoes de transporte tradicional. Protocolos de streaming adaptativo - como DASH (Dynamic Adaptive Streaming over HTTP) e HLS (HTTP Live Streaming) a ajustam a qualidade do video conforme as condicoes instantaneas de rede, fragmentando o conteudo em pequenos segmentos com diferentes bitrate. Dessa forma, o player pode alternar entre versoes de maior ou menor qualidade para evitar interrupcoes, maximizando a experiencia do usuario final. Por fim, ha um movimento crescente em direcao a redes auto-aprendizes, que utilizam tecnicas de inteligencia artificial e machine learning para prever padroes de trafego, identificar anomalias de seguranca e otimizar rotas com base em metricas multidimensionais. Modelos preditivos podem estimar picos de uso antes que eles ocorram, permitindo a realocacao de recursos de forma proativa, enquanto algoritmos de deteccao de intrusao comportamental analisam fluxos de dados em busca de desvios sutis que possam indicar ataques de negacao de servico ou exfiltracao de informacoes. Em suma, a evolucao das redes de comunicacao continua vertiginosa, movida pela necessidade de entregar cada vez mais dados - frequentemente em tempo real - com altissimos niveis de confiabilidade e eficiencia. Dos protocolos basicos de roteamento e controle de congestionamento as arquiteturas definidas por software e inteligencia artificial, cada camada do modelo de rede foi aprimorada para enfrentar novos requisitos e cenarios complexos, garantindo que a espinha dorsal da era digital permaneça robusta, agil e escalavel.";

    string msg_transmitir = "oi" ; 

    vector<char> msg ; 

    for(auto a : msg_transmitir) msg.push_back(a) ; 

    if(pc.sendData(socket, msg)) cout << "Mensagem enviada com sucesso\n" ; 
    else cout << "algum erro ocorreu\n" ; 

    if(pc.Disconnect(socket)) cout << "deu bom\n" ; 

    return 0;
}
