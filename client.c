#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

//Tamanho maximo da mensagem de 30kb
#define MAXLINE 30000
#define SA struct sockaddr
#define SERVERPORT 3500

typedef enum{false, true} bool;

//Função que cria o socket e envia mensagem para o servidor
void message_to_server(char*, char*);

int main(void){
	char	client_ip[16], server_ip[16], mensagem[MAXLINE+1];
	char	aux_mensagem[MAXLINE+1];
	FILE	*fp, *sendtxt;
	bool	client_running = false;

	//Usa uma systemcall para pegar o IP do servidor
	fp = popen("hostname -I", "r");
	fgets(client_ip, 16, fp);
	client_ip[strlen(client_ip) - 1] = '\0';

	printf("Utilize a tag quit() para sair do chat\n");

	//Pede para o cliente o IP do servidor no qual quer fazer contato
	printf("Entre com o IP do servidor: ");
	scanf("%s", server_ip);

	//Infomações básicas do cliente
	printf("\n--------------- Client Info ---------------\n");
	printf("Client:: -> Client IP = %s\n", client_ip);
	printf("Client:: -> Server IP = %s\n", server_ip);
	printf("Client:: -> PORT = %d\n", (int)SERVERPORT);
	printf("\n------------------- Chat ------------------\n");

	//Envia uma mensagem para o servidor avisando que o Cliente
	//está conectado e querendo comunicar
	strcpy(mensagem, client_ip);
	strcat(mensagem, "está CONECTANDO.\n");
	message_to_server(mensagem, server_ip);
	client_running = true;

	for(;;){
		strcpy(mensagem, client_ip);
		printf("Cliente:: -> Mensagem: ");

		//Le a mensagem do cliente e salva em uma variavel auxiliar
		setbuf(stdin, NULL);
		fgets(aux_mensagem, sizeof(aux_mensagem), stdin);

		//Caso o cliente use a tag quit(), cliente avisa o servidor
		//que não vai mais comunicar
		if(!strcmp(aux_mensagem,"quit()\n")){
			strcat(mensagem, "está SAINDO.\n");
			printf("Saindo.\n");
			client_running = false;
		}
		//Caso o cliente use a tag text(), cliente envia a informaçãonce
		//que estava salva em um .txt de até 30kbits
		else if(!strcmp(aux_mensagem,"text()\n")){
			strcpy(aux_mensagem, "");
			strcat(mensagem, ": TEXTO\n-----\n");
			if((sendtxt = fopen("send_data.txt", "r")) == NULL){
				printf("Não existe arquivo.\n");
			}
			while(fgets(aux_mensagem, MAXLINE+1, sendtxt))
				strcat(mensagem, aux_mensagem);
			strcat(mensagem, "-----\n");
		}
		//Se não o cliente esta apenas enviando mensagens normais
		else{
			strcat(mensagem, ": ");
			strcat(mensagem, aux_mensagem);
		}

		message_to_server(mensagem, server_ip);

		//Se cliente avisou o servidor que não que mais comunicar
		//então fecha o processo do cliente
		if(!client_running)
			break;
	}
}

void message_to_server(char* mensagem, char* server_ip){
	int			socket_client;
	struct	sockaddr_in servaddr;

	if((socket_client = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Cliente:: -> Erro ao criar o socket.\n");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVERPORT);

	if(inet_pton(AF_INET, server_ip, &servaddr.sin_addr) <= 0){
		printf("Cliente:: -> Erro com o inet para o ip %s\n", server_ip);
		exit(1);
	}

	if(connect(socket_client, (SA *) &servaddr, sizeof(servaddr)) < 0){
		printf("Cliente:: -> Error ao conectar com o servidor\n");
		exit(1);
	}

	//Envia mensagem e fecha o socket esperando uma nova entrada
	//de mensagem do cliente
	send(socket_client, mensagem, strlen(mensagem) + 1, 0);
	close(socket_client);
}
