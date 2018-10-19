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

int main(void){
	int			socket_client, port = SERVERPORT;
	char		client_ip[16], server_ip[16], mensagem[MAXLINE+1], aux_mensagem[MAXLINE+1];
	struct	sockaddr_in servaddr;
	FILE		*fp;

	//Get IP address and save
	fp = popen("hostname -I", "r");
	fgets(client_ip, 16, fp);
	client_ip[strlen(client_ip) - 1] = ' ';

	printf("Utilize a tag quit() para sair do chat\n");

	printf("Entre com o IP do servidor: ");
	scanf("%s", server_ip);

	for(;;){
		printf("Cliente:: -> Mensagem: ");

		setbuf(stdin, NULL);
		fgets(aux_mensagem, sizeof(aux_mensagem), stdin);

		if(!strcmp(aux_mensagem,"quit()\n")){
			printf("Saindo.\n");
			exit(0);
		}
		else{
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

			strcat(mensagem, client_ip);
			strcat(mensagem, ": ");
			strcat(mensagem, aux_mensagem);
			send(socket_client, mensagem, strlen(mensagem) + 1, 0);
			strcpy(mensagem, "");
		}
	}
}
