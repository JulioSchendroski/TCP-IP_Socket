//Implementação do servidor responsável em ficar escutando novas conexões de clientes
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void* cliente(void* args){
    //Realizando conversão de void para int
    int cliente = *((int*)args);
    int rede_socket;

    //Estrutura utilizada para lidar com endereços IPv4 
    struct sockaddr_in servidorAddr;

    //Específica-se o endereço que irá receber as sinalizações de comunicação
    servidorAddr.sin_addr.s_addr = INADDR_ANY;

    //Específicando que o servidor addr será do tipo Ipv4
    servidorAddr.sin_family = AF_INET;

    //Específicando a porta do TCP (8989)
    //o método htons é utilizado para manter o arranjo dos bytes enviados na rede
    servidorAddr.sin_port = htons(8989);

    //socket(domain, type, protocol);
    //Criando um socket de cliente, como argumentos, passamos 2 por ser o tipo de comunicação, no caso 2 representa Ipv4
    //O segundo argumento define o tipo de protocolo, como trata-se de protocolo TCP, seu dicíonario corresponde a 1
    //O último argumento representa o protocolo do IP (número 0), número presente no cabeçalho IP de pacotes TCPs.
    rede_socket = socket(2,1,0);


    //Inicializando a conexão com o socket do servidor utilizando o método connect, responsável por manter a conexão entre dois sockets, se não conseguir se conectar, retorna -1
    int status_conexão = connect(rede_socket, (struct sockaddr*) &servidorAddr, sizeof(servidorAddr));

    //Verificação se há erros de conexão
    if(status_conexão < 0){
        printf("\nErro de conexão!");
        return 0;
    } 

    printf("\nConexão estabelecida!");

    //O método send envia dos dados presentes na thread de cliente para o socket de servidor
    send(rede_socket, &cliente, sizeof(cliente), 0);

    //Finaliza a conexão com o socket de servidor
    close(rede_socket);
    pthread_exit(NULL);
 
    return 0;
}


int main(){
    int escolhaCliente = 0;

    printf("1: Leitor\n");
    printf("2: Escritor\n");

    scanf("%d", &escolhaCliente);
    pthread_t threadCliente;

    //Estabelencendo conexão com base no input do usuário
    switch(escolhaCliente){
        case 1: {
            int requisicao_cliente = 1;

            //Criando thread do cliente
            pthread_create(&threadCliente, NULL, cliente, &requisicao_cliente);
            sleep(10);
            break;
        }

        case 2: {
            int requisicao_cliente = 2;

            //Criando thread do cliente
            pthread_create(&threadCliente, NULL, cliente, &requisicao_cliente);
            sleep(10);
            break;
        }

        default:{
            printf("\nEntrada inválida!");
            break;
        }
    }

    //Esperar terminar a thread
    pthread_join(threadCliente, NULL);
}