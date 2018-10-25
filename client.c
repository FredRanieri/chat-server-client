#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 4096
#define SA struct sockaddr
#define SERVERPORT 3500

typedef enum{false, true} bool;

void message_to_server(char*, char*);

int main(void){
	char		client_ip[16], server_ip[16], mensagem[MAXLINE+1], aux_mensagem[MAXLINE+1];
	FILE		*fp;
	bool client_running = false;

	//Get IP address and save
	fp = popen("hostname -I", "r");
	fgets(client_ip, 16, fp);
	client_ip[strlen(client_ip) - 1] = '\0';

	printf("Utilize a tag quit() para sair do chat\n");

	printf("Entre com o IP do servidor: ");
	scanf("%s", server_ip);

	printf("\n--------------- Client Info ---------------\n");
	printf("Client:: -> Client IP = %s\n", client_ip);
	printf("Client:: -> Server IP = %s\n", server_ip);
	printf("Client:: -> PORT = %d\n", (int)SERVERPORT);
	printf("\n------------------- Chat ------------------\n");

	strcpy(mensagem, client_ip);
	strcat(mensagem, "está CONECTANDO.\n");

	message_to_server(mensagem, server_ip);
	client_running = true;

	for(;;){
		strcpy(mensagem, client_ip);
		printf("Cliente:: -> Mensagem: ");

		setbuf(stdin, NULL);
		fgets(aux_mensagem, sizeof(aux_mensagem), stdin);

		if(!strcmp(aux_mensagem,"quit()\n")){
			strcat(mensagem, "está SAINDO.\n");
			printf("Saindo.\n");
			client_running = false;
		}
		else{
			strcat(mensagem, ": ");
			strcat(mensagem, aux_mensagem);
		}
		message_to_server(mensagem, server_ip);
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
	send(socket_client, mensagem, strlen(mensagem) + 1, 0);
	close(socket_client);
}
