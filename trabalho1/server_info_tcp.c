#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define DBFILENAME "db.csv"

#define BACKLOG 100

#define ITEMLIM 8000

#define BUFLEN 1024
#define MAXSTRLEN 256


typedef enum
{
	FALSE = 0,
	TRUE = 1
} bool_t;



/* === Estrutura de dados dos estabelecimentos ============================ */

typedef struct item {
	int id;
	int posx;
	int posy;
	char categoria[MAXSTRLEN];
	char nome[MAXSTRLEN];
	char endereco[MAXSTRLEN];
	int pontuacao;
	int votos;
} item_t;



/* === Ferramentas para tratamento de erros =============================== */

typedef enum
{
    NO_ERROR = 0,
    USAGE_ERROR,
    PORT_OUT_RANGE,
    DB_FEWER_ROWS,
    BIND_ERROR,
    MYERROR_LIM
    
} my_error_t;

const char* my_error_desc[] =
{
    "",
    "usage: ./server_info_tcp <port number>",
    "error: port number must be between 1 and 1024",
    "error: database constains less rows than specified",
    "error: failed to bind",
    ""
};

void pMyError(my_error_t e, const char *function)
{
    if(0 < e && e < MYERROR_LIM) {
    	fprintf(stderr, "%s: %s.\n", function, my_error_desc[e]);
    }
}



/* === Estruturas de dados com as informações dos estabelecimentos ======== */

static item_t *lista;
static int lista_len;



/* === Funções auxiliares de depuração ==================================== */

void DEBUG_printItem(item_t *item)
{
	printf("*** item ***\n");
	printf("Id: %d\t", item->id);
	printf("Posição: %d,%d\n", item->posx, item->posy);
	printf("Categoria: %s\t", item->categoria);
	printf("Nome: %s\n", item->nome);
	printf("Endereço: %s\n", item->endereco);
	printf("Pontos acumul.: %d\t", item->pontuacao);
	printf("Total de votos: %d\n", item->votos);
	printf("\n");
}



/* === Funções auxiliares ================================================= */

void readItemInfo(item_t *item, char *buf)
{
	int i = 0;
	char *tok;

	tok = strtok(buf,";");
	while (tok != NULL) {
		switch (i) {
			case 0:
				item->id = atoi(tok);
				break;
			case 1:
				item->posx = atoi(tok);
				break;
			case 2:
				item->posy = atoi(tok);
				break;
			case 3:
				strncpy(item->categoria, tok, MAXSTRLEN);
				break;
			case 4:
				strncpy(item->nome, tok, MAXSTRLEN);
				break;
			case 5:
				strncpy(item->endereco, tok, MAXSTRLEN);
				break;
			case 6:
				item->pontuacao = atoi(tok);
				break;
			case 7:
				item->votos = atoi(tok);
				break;
		}
		i++;
		tok = strtok(NULL,";");
	}
}

void readDB(void)
/* === Acesso às informações dos estabelecimentos ========================= */
{
	FILE *db;
	char buf[BUFLEN];
	int i;

	db = fopen (DBFILENAME,"r");
	if (db == NULL) {
		perror("failed to open the database");
		exit(EXIT_FAILURE);
	}

	/* lê o número de estabelecimentos */
	fgets(buf, BUFLEN, db);
	lista_len = atoi(buf);
	
	/* cria a lista de estabelecimentos */
	lista = calloc(lista_len, sizeof(item_t));

	/* lê as informações de cada estabelecimento */
	i = lista_len;
	while (!feof(db) && i > 0) {
		fgets(buf, BUFLEN, db);
		readItemInfo(&lista[lista_len - (i--)], buf);
	}
	/* tratamento de erro para quando temos menos estabelecimentos do que N */
	if (i > 0) {
		pMyError(DB_FEWER_ROWS, __func__);
		exit(EXIT_FAILURE);
	}

	fclose(db);
}

void writeDB(void)
{
	FILE *db;
	char buf[BUFLEN];
	int i;

	db = fopen (DBFILENAME,"w");
	if (db == NULL) {
		perror("failed to write to the database");
		exit(EXIT_FAILURE);
	}

	fprintf(db, "%d\n", lista_len);

	for (i = 0; i < lista_len; i++) {
		fprintf(db, "%.4d;", lista[i].id);
		fprintf(db, "%d;%d;", lista[i].posx, lista[i].posy);
		fprintf(db, "%s;", lista[i].categoria);
		fprintf(db, "%s;", lista[i].nome);
		fprintf(db, "%s;", lista[i].endereco);
		fprintf(db, "%d;", lista[i].pontuacao);
		fprintf(db, "%d\n", lista[i].votos);

	}

	fprintf(db, "\n");

	fclose(db);
}

int buscarD100C(int *busca, const int x, const int y, char *categoria)
/*
 * desc		:	Busca em um raio de 100 unidades da posição (x,y) todos os
 *				estabelecimentos da categoria dada (ou de qualquer categoria).
 *
 * params	:	1.	Vetor de inteiros onde serão armazenados os índices na
 *					na lista dos estabelecimentos encontrados.
 *				2.	Coordenada x da posição do centro da busca.
 *				3.	Coordenada y da posição do centro da busca.
 *				4.	Categoria dos estabelecimentos a serem buscados; caso
 *					este parâmetro seja NULL, procura-se por qualquer
 *					categoria de estabelecimento.
 *
 * output	:	O número de estabelecimentos encontrados pela busca.
 */
{
	int out = 0;
	int dx, dy;
	int i;

	readDB();

	for (i = 0; i < lista_len; i++) {
		dx = lista[i].posx - x;
		dy = lista[i].posy - y;
		if (dx*dx + dy*dy <= 10000
		&& (categoria == NULL
		|| strcmp(lista[i].categoria, categoria) == 0)) {
			busca[out++] = lista[i].id;
		}

	}

	writeDB();
	free(lista);
	lista_len = 0;

	return out;
}

void printID(int id, int S)
{
	bool_t exist_id = FALSE;
	char *error_msg = "Não existe o estabelecimento com o id dado.\n";
	char buf[BUFLEN];
	int i;

	readDB();

	for (i = 0; i < lista_len; i++) {
		if (lista[i].id == id) {
			exist_id = TRUE;
			snprintf(buf, BUFLEN,
				"<<< %s >>>\n  ID: %.4d\tCategoria: %s\n  Endereço: %s\n  Posição: (%d,%d)\tNota: %.2f\n",
				lista[i].nome, lista[i].id, lista[i].categoria,
				lista[i].endereco, lista[i].posx, lista[i].posy,
				(float)(lista[i].pontuacao / lista[i].votos));
			if (send(S, buf, strlen(buf), 0) == -1) {
         	   perror("send");
        	}
		}
	}

	if (!exist_id) {
		if (send(S, error_msg, strlen(error_msg), 0) == -1) {
            perror("send");
        }
	}

	writeDB();
	free(lista);
	lista_len = 0;
}

int bindTCP(char *port)
{
	int socket_fd;
	struct addrinfo hints, *servinfo, *p;
	int status;
	int optval = 1;


	socket_fd = -1;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "%s: getaddrinfo error: %s\n", __func__, gai_strerror(status));
		exit(EXIT_FAILURE);
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("server_info_tcp: socket");
			continue;
		}

		if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1) {
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}

		if (bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_fd);
			perror("server_info_tcp: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		pMyError(BIND_ERROR, __func__);
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(servinfo);

	return socket_fd;
}




/* ========================================================================
 * === MAIN ===============================================================
 * ======================================================================== */

int main(int argc, char *argv[])
{
	char buf[BUFLEN];
	int len;
	int i;

	int *busca;

	char *port;
	int listen_socket, connect_sock;
	struct sockaddr_storage remote_st;
	socklen_t st_len;

	struct sockaddr_in si;
	socklen_t si_len;
	
	struct sigaction sa;
	pid_t pid;

	char *tok;
	int cli_posx, cli_posy;


	/* verificando argumentos */
	if (argc < 2) {
		pMyError(USAGE_ERROR, __func__);
		exit(EXIT_FAILURE);
	}

	i = atoi(argv[1]);
	if (0 >= i && i > 1024) {
		pMyError(PORT_OUT_RANGE, __func__);
		exit(EXIT_FAILURE);
	}

	/* obtendo a porta */
	i = strlen(argv[1]);
	port = calloc(i, sizeof(char));
	strncpy(port, argv[1], i);

	busca = calloc(ITEMLIM, sizeof(int));


/* ===  =================================================================== */

	listen_socket = bindTCP(port);

	/* espera por conexões pelo listen_socket */ 
	if (listen(listen_socket, BACKLOG) == -1) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	/* configurando o handler para encerrar os procesos-zumbis */
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDWAIT;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }


    while(1) {
    	
    	/* aceita a conexão de um cliente por um socket novo new_s */
		if ((connect_sock = accept(listen_socket, (struct sockaddr *)&remote_st, &st_len)) < 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
		
		/* faz o fork do processo */
		pid = fork();
		if (pid < 0) {
			perror("fork");
			exit(EXIT_FAILURE);
		}
		
		/* caso o processo seja o filho */
		if (pid == 0) {
		
			/* fecha o socket que está esperando por novos clientes */
			close(listen_socket);
		
			/* coleta as informações do socket e imprime na stdout */
			si_len = sizeof(si);
			if (getpeername(connect_sock, (struct sockaddr *)&si, &si_len) < 0) {
				perror("getpeername");
				close(connect_sock);
				exit(EXIT_FAILURE);
			}
			inet_ntop(AF_INET, &(si.sin_addr), buf, INET_ADDRSTRLEN);
			printf("IP address: %s; Port number: %d\n", buf, ntohs(si.sin_port));
		
			/* main loop */
			while (len = recv(connect_sock, buf, sizeof(buf), 0)) {
				if (len > 0) {
					tok = strtok(buf, " ");
					if (strcmp(tok, "posicao") == 0) {
						cli_posx = atoi(strtok(NULL, ","));
						cli_posy = atoi(strtok(NULL, ","));
					}
					printf("%d,%d\n", cli_posx,cli_posy);
				}
			}
		
			close(connect_sock);
		}
		else {
			close(connect_sock);
		}
	}

/* ===  =================================================================== */

	free(port);
	free(busca);

	return 0;
}