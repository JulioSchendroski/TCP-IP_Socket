//Implementação do servidor responsável em ficar escutando novas conexões de clientes
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define QUANTIDADE_CLIENTES 50
#define TRUE 1


//Declarando variáveis do Semáforo e Threads
sem_t semaforoA, semaforoB;
pthread_t tamanhoSocketEscritura[100];
pthread_t tamanhoSocketLeitura[100];
int controle = 0;

//Função para lidar com o modo leitura que o cliente irá se conectar
void* leitor(void* param){

    sem_wait(&semaforoA);
    controle++;

    //Trava o semáforo B esperando a conexão de um outro leitor
    if (controle == 1){
        sem_wait(&semaforoB);
    }

    //Desbloqueia o semáforo A, uma vez que o semáforo B está bloqueado
    sem_post(&semaforoA);
    printf("\n%d Leitor Conectado!", controle);
    
    //Mantém o cliente leitor conectado por 10 segundos e logo após o semáforo A é fechado novamente
    sleep(5);
    sem_wait(&semaforoA);

    //Faz o decremento do contador de controle, permitindo que o semáforo B seja liberado
    controle--;

    if(controle == 0){
        sem_post(&semaforoB);
    }

    sem_post(&semaforoA);
    printf("\n%d Leitor saindo...", controle  + 1);

    //Liberando a Thread;
    pthread_exit(NULL);
}


//Função para lidar com o modo escritura que o cliente irá se conectar
void* escritor(void *param){
    printf("\nEscritor está se conectando...");

    //Bloqueia o semáforo B para exibir mensagem de retorno para o cliente
    sem_wait(&semaforoB);

    printf("\nEscritor entrou!");

    //Desbloqueia o semáforo B
    sem_post(&semaforoB);

    printf("\nEscritor saindo!");
    pthread_exit(NULL);
}


int main(){

    //Inicializando um servidor socket e um socket;
    int servidorSocket, novoSocket;

    //Criando um armazenador de sockets com tamanho necessário para armazenar endereços tipo Ipv4 ou Ipv6
    struct sockaddr_storage armazenadorServidor;

    //Estrutura utilizada para lidar com endereços IPv4 
    struct sockaddr_in servidorAddr;

    //Tamanho do socket de serverAddr
    socklen_t tamanho_addr;

    //Inicialziando semáforos A e B para lidar com acesso de novos clientes
    sem_init(&semaforoA, 0, 1);
    sem_init(&semaforoB, 0, 1);

    //socket(domain, type, protocol);
    //Criando um socket de server, como argumentos, passamos 2 por ser o tipo de comunicação, no caso 2 representa Ipv4
    //O segundo argumento define o tipo de protocolo, como trata-se de protocolo TCP, seu dicíonario corresponde a 1
    //O último argumento representa o protocolo do IP (número 0), número presente no cabeçalho IP de pacotes TCPs.
    servidorSocket = socket(AF_INET, SOCK_STREAM, 0);

    //Específica-se o endereço que irá receber as sinalizações de comunicação
    servidorAddr.sin_addr.s_addr = INADDR_ANY;

    //Específicando que o servidor addr será do tipo Ipv4
    servidorAddr.sin_family = AF_INET;

    //Específicando a porta do TCP (8989)
    //o método htons é utilizado para manter o arranjo dos bytes enviados na rede
    servidorAddr.sin_port = htons(8989);

    //A função bind liga o socket ao endereço e ao número da porta específicada anteriormente em addr
    bind(servidorSocket, (struct sockaddr*)&servidorAddr,sizeof(servidorAddr));

    //A função listen coloca o socket no modo servidor, define a quantidade de solicitações de novas conexões que podem ser enfileiradas
    if(listen(servidorSocket, QUANTIDADE_CLIENTES) == 0){
        printf("\nAguardando conexões: ");
    }else{
        printf("\nHouve algum problema!");
    }

    
    int i = 0;

    while(1){
  
        tamanho_addr = sizeof(armazenadorServidor);
	

        //A função accept retorna um novo socket para ser usado com aquela conexão e, o endereço do cliente
        novoSocket = accept(servidorSocket, (struct sockaddr*) &armazenadorServidor, &tamanho_addr);
	
	int escolhaDoCliente = 0;
	
        //A função recv é utilizada para obter os dados para socket
        recv(novoSocket, &escolhaDoCliente, sizeof(escolhaDoCliente), 0);

        //Casos em que cliente queria acesar como modo leitor ou escritor
        if(escolhaDoCliente == 1){
            if(pthread_create(&tamanhoSocketLeitura[i++], NULL, leitor, &novoSocket) != 0){
                printf("Falha ao criar a thread de leitor");
            }
        }else if (escolhaDoCliente == 2){
            if(pthread_create(&tamanhoSocketEscritura[i++], NULL, escritor, &novoSocket) != 0){
                printf("Falha ao criar a thread de escritor");
            }
        }
        
        //Garantindo que todas threads serão finalizadas, bloquando quem a chamou 
        if(i >= QUANTIDADE_CLIENTES){
            i = 0;
            while(i < QUANTIDADE_CLIENTES){
                pthread_join(tamanhoSocketLeitura[i++], NULL);
                pthread_join(tamanhoSocketEscritura[i++], NULL);
            }
            i = 0;
        }
    }
    
    return 0;   
}
