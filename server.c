#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define MAXLINE 1024
#define SA struct sockaddr
#define SERVERPORT 3500
#define MAX_PENDING 1

typedef enum{false, true} bool;

int main(){
	int			socket_server, new_socket, tamanho;
	long		server_try;
	char		ip[16], mensagem[MAXLINE+1];
	struct 	sockaddr_in servaddr;
	bool		server_running = false;
	FILE		*fp;

	//Get IP address and save
	fp = popen("hostname -I", "r");
	fgets(ip, 16, fp);
	ip[strlen(ip) - 1] = ' ';

	printf("Servidor:: -> Tentando conexao\n");

	for(server_try = 0; server_try < 2000000; server_try++){
		bzero((char *)&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = INADDR_ANY;
		servaddr.sin_port = htons(port);

		if((socket_server = socket(AF_INET, SOCK_STREAM, 0)) < 0){
			printf("Servidor:: -> Error na criação do socket!\n");
			exit(1);
		}

		if(!(bind(socket_server, (SA *)&servaddr, sizeof(servaddr)) < 0)){
      server_running = true;
      break;
		}
		close(socket_server);
  }

	if(!server_running){
    printf("Servidor:: -> Falha na conexao.\n");
    exit(1);
  }

	printf("Servidor:: -> Servidor criado.\n");
	printf("\n--------------- Server Info ---------------\n");
	printf("Servidor:: -> IP = %s\n", ip);
	printf("Servidor:: -> PORT = %d\n", (int)SERVERPORT);
	printf("\n------------------- Chat ------------------\n");

	listen(socket_server, MAX_PENDING);

	for(;;){
		if((new_socket = accept(socket_server, (SA *)&servaddr, &tamanho)) < 0){
			printf("Servidor:: -> Erro na conexao com cliente.\n");
			exit(1);
		}

		recv(new_socket, mensagem, sizeof(mensagem), 0);

		printf("[%s] %s", __TIME__, mensagem);

		close(new_socket);
	}
}
