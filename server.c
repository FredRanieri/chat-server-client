#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

//Tamanho maximo da mensagem de 30kb
#define MAXLINE 30000
#define SA struct sockaddr
#define SERVERPORT 3500
#define MAXPENDING 5

typedef enum{false, true} bool;

//Função para fazer o registro das mensagens recebidas
void save_to_database(char*, FILE*);

int main(){
	int			socket_server, new_socket, tamanho;
	long		server_try;
	char		ip[16], mensagem[MAXLINE+1];
	struct 	sockaddr_in servaddr;
	bool		server_running = false;
	FILE		*fp, *datarecord;
	time_t	time_now;

	//Chamada para configurar horario local
	time (&time_now);

	//Usa uma systemcall para pegar o IP do servidor
	fp = popen("hostname -I", "r");
	fgets(ip, 16, fp);
	ip[strlen(ip) - 1] = ' ';

	printf("Servidor:: -> Tentando conexao\n");

	//Bloco de codigo para caso ocorra algum erro na conexão
	//do servidor ele tente novamente
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

	//Testa se o servidor conseguiu conexão após as tentativas
	if(!server_running){
    printf("Servidor:: -> Falha na conexao.\n");
    exit(1);
  }

	//Infomações básicas do seu servidor
	printf("Servidor:: -> Servidor criado.\n");
	printf("\n--------------- Server Info ---------------\n");
	printf("Servidor:: -> IP = %s\n", ip);
	printf("Servidor:: -> PORT = %d\n", (int)SERVERPORT);
	printf("\n------------------- Chat ------------------\n");

	strcpy(mensagem, "Servidor:: -> INICIADO -> ");
	strcat(mensagem, ctime(&time_now));

	//Salva no registro a data e a hora que o servidor foi criado
	save_to_database(mensagem, datarecord);

	//Caso chegue mais de uma mensagem ao mesmo tempo fica na fila(Max. 5)
	listen(socket_server, MAXPENDING);

	for(;;){
		if((new_socket = accept(socket_server, (SA *)&servaddr, &tamanho)) < 0){
			printf("Servidor:: -> Erro na conexao com cliente.\n");
			exit(1);
		}

		//Recebe mensagens do cliente
		recv(new_socket, mensagem, sizeof(mensagem), 0);

		//Mostra na tela a mensagem recebida e salva no registro
		printf("[%s] %s", __TIME__, mensagem);
		save_to_database(mensagem, datarecord);

		//Fecha conexão com o cliente para pode aguardar uma nova mensagem
		close(new_socket);
	}
}

void save_to_database(char* mensagem, FILE* datarecord){
	//Abre um arquivo de texto para o registro, caso não exista cria um
	if((datarecord = fopen("data.txt", "a")) == NULL){
		printf("Problema no registro\n");
		exit(1);
	}

	//Salva informações no registro
	fprintf(datarecord, "%s", mensagem);
	fclose(datarecord);
}
