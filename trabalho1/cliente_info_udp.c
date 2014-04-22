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



#define BUFLEN 1024



/* === Tipos auxiliares =================================================== */

typedef enum
{
	FALSE = 0,
	TRUE = 1
} bool_t;



/* === Ferramentas para tratamento de erros =============================== */

typedef enum {
    NO_ERROR = 0,
    USAGE_ERROR,
    PORT_OUT_RANGE,
    CONNECT_ERROR,
    MYERROR_LIM

} my_error_t;

const char* my_error_desc[] =
{
    "",
    "usage: ./cliente_info_tcp <port number>",
    "error: port number must be between 1 and 1024",
    "error: failed to connect"
    ""
};

void pMyError(my_error_t e, const char *function)
{
    if(NO_ERROR < e && e < MYERROR_LIM) {
    	fprintf(stderr, "%s: %s.\n", function, my_error_desc[e]);
    }
}



static int msg_seq;



/* ========================================================================
 * === Pragmas das funções ================================================
 * ======================================================================== */

/* === Funções auxiliares de networking =================================== */
int msgSeq(void);
int connectUDP(char *endIP, char *port);



/* ========================================================================
 * === MAIN ===============================================================
 * ======================================================================== */

int main(int argc, char *argv[])
{
	int sock;

	fd_set rfds0, rfds1;
	struct timeval tv;

	int maxfd, rval;

	char msg[BUFLEN];
	char cmd[BUFLEN];
	char buf[BUFLEN];
	int len;

	int i, n;

	bool_t server_ready;

	fd_set tmp_fds;
	char ack_msg[9];
	struct timeval tv1;

	tv1.tv_sec = 1;
	tv1.tv_usec = 0;



	/* verificando argumentos */
	if (argc < 3) {
		pMyError(USAGE_ERROR, __func__);
		exit(EXIT_FAILURE);
	}

	i = atoi(argv[2]);
	if (0 >= i && i > 1024) {
		pMyError(PORT_OUT_RANGE, __func__);
		exit(EXIT_FAILURE);
	}

	sock = connectUDP(argv[1], argv[2]);

	maxfd = fileno(stdin);
	maxfd = (maxfd < sock) ? sock : maxfd;

	FD_ZERO(&rfds0);

	FD_SET(fileno(stdin), &rfds0);
	FD_SET(sock, &rfds0);

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	memset(buf, 0, sizeof(buf));

	server_ready = TRUE;

	while (1) {

		rfds1 = rfds0;

        if ((rval = select(maxfd + 1, &rfds1, NULL, NULL, &tv)) == -1) {
			perror("select");
			exit(EXIT_FAILURE);
		}
		else if (rval > 0) {

			if (FD_ISSET(fileno(stdin), &rfds1) && server_ready) {

				memset(cmd, 0, sizeof(cmd));
				fgets(cmd, BUFLEN, stdin);

				/* encerra o cliente */
				if (strcmp(cmd, "sair\n") == 0) {
					break;
				}

				/* etiqueta a mensagem com o número de sequência */
				sprintf(msg, "%.6d", msgSeq());
				strncat(msg, cmd, strlen(cmd));

				if (send(sock, msg, strlen(msg), 0) == -1) {
					perror("send");
				}

				FD_ZERO(&tmp_fds);

				/* espera pelo ACK do servidor */
				while (1) {
					FD_SET(sock, &tmp_fds);
					i = select(sock+1, &tmp_fds, NULL, NULL, &tv1);

					if ( i > 0) {

						memset(buf, 0, sizeof(buf));
						if ((len = recv(sock, buf, 9, 0)) == -1) {
							perror("recv");
						}

						sprintf(ack_msg, "ACK%.6d", msg_seq);
						if (strncmp(ack_msg, buf, (size_t)(9)) == 0) {
							break;
						}
					}
					else if (i <= 0) {
						puts("Sem resposta do servidor.");
						break;
					}
				}

				server_ready = FALSE;

			}
			else if (FD_ISSET(sock, &rfds1)) {
				memset(buf, 0, sizeof(buf));
				if ((len = recv(sock, buf, BUFLEN, 0)) == -1) {
					perror("recv");
				}

				if (strstr(buf, "SERVER_READY\n\r") != NULL) {
					server_ready = TRUE;
				}
				else if (strcmp(buf, "\n") != 0 || strcmp(buf, "\r") != 0) {
					printf("%s", buf);
				}
			}

		}

	}

	return 0;
}



/* ========================================================================
 * === Implementação das funções ==========================================
 * ======================================================================== */

/* === Funções auxiliares de networking =================================== */

int msgSeq(void)
/*
 * desc		:	Retorna um inteiro entre 0 e 999 999 para ser usado como
 * 				identificador de mensagens.
 *
 * params	:	Nenhum.
 *
 * output	:	Inteiro (mod 1 000 000).
 */
 {
	if (++msg_seq > 999999) {
		msg_seq = 0;
	}

	return msg_seq;
}


int connectUDP(char *endIP, char *port)
/*
 * desc		:	Conecta a um endereço IP e porta a um socket a ser retornado.
 *
 * params	:	1.	String contendo o endereço IP a ser usado.
 * 				2.	String contendo a porta a ser usada.
 *
 * output	:	O socket da conexão criada.
 */
{
	int socket_fd;
	struct addrinfo hints, *servinfo, *p;
	int status;

	/* inicialmente retornando erro */
	socket_fd = -1;

	/* estrutura a ser usada para obter um endereço IP */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;			/* IPv4 */
	hints.ai_socktype = SOCK_DGRAM;		/* UDP */

	/* obtem a lista ligada com as associações possíveis */
	if ((status = getaddrinfo(endIP, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "%s: getaddrinfo error: %s\n", __func__, gai_strerror(status));
		exit(EXIT_FAILURE);
	}

	/* percorre servinfo tentando se conectar */
	for (p = servinfo; p != NULL; p = p->ai_next) {

		/* tenta obter um socket */
		if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("socket");
			continue;
		}

		/* tenta se conectar ao endereço IP dado */
		if (connect(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_fd);
			perror("connect");
			continue;
		}

		break;
	}

	/* tratamento de erro caso o bind falhe */
	if (p == NULL) {
		pMyError(CONNECT_ERROR, __func__);
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(servinfo);

	return socket_fd;
}
