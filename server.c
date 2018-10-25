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

void save_to_database(char*, FILE*);

int main(){
	int			socket_server, new_socket, tamanho;
	long		server_try;
	char		ip[16], mensagem[MAXLINE+1];
	struct 	sockaddr_in servaddr;
	bool		server_running = false;
	FILE		*fp, *datarecord;
	time_t	time_now;
	time (&time_now);

	//Get IP address and save
	fp = popen("hostname -I", "r");
	fgets(ip, 16, fp);
	ip[strlen(ip) - 1] = ' ';

	printf("Servidor:: -> Tentando conexao\n");

	for(server_try = 1; server_try < 20000000; server_try++){
		if((server_try % 5000000) == 0)
			printf("Servidor:: -> Ainda tentando conexao...\n");

		bzero((char *)&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = INADDR_ANY;
		servaddr.sin_port = htons(SERVERPORT);

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

	strcpy(mensagem, "Servidor:: -> INICIADO -> ");
	strcat(mensagem, ctime(&time_now));
	strcat(mensagem, "\n");
	save_to_database(mensagem, datarecord);

	listen(socket_server, MAX_PENDING);

	for(;;){
		if((new_socket = accept(socket_server, (SA *)&servaddr, &tamanho)) < 0){
			printf("Servidor:: -> Erro na conexao com cliente.\n");
			exit(1);
		}

		recv(new_socket, mensagem, sizeof(mensagem), 0);

		printf("[%s] %s", __TIME__, mensagem);
		save_to_database(mensagem, datarecord);
		close(new_socket);
	}
}

void save_to_database(char* mensagem, FILE* datarecord){
	if((datarecord = fopen("data.txt", "a")) == NULL){
		printf("Problema no registro\n");
		exit(1);
	};

	fprintf(datarecord, "%s", mensagem);
	fclose(datarecord);
}
