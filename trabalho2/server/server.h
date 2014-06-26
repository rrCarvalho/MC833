/* myNetworking header
 *
 * MC833 - Programacao em Redes de Computadores
 * Exercicio 7: Servidor de Eco TCP/UDP
 *
 * Autor: Raul Rabelo Carvalho, 105607
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <poll.h>

#define NAMESIZE 256

/* Tamanho maximo dos buffers: */
#define MAXLINE 4096
#define BUFSIZE 8192

/* Porta de comunicacao padrao: */
#define STD_PORT_NUM  10000
#define STD_PORT_STR  "10000"

#define BACKLOG 1024

#define SA  struct sockaddr

/* Macros para o segundo parametro de Shutdown: */
#define SHUTRECV 0 /* further receives are disallowed */
#define SHUTSEND 1 /* further sends are disallowed */
#define SHUTBOTH 2 /* further sends and receives are disallowed */

#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

/* Definicao de um tipo booleano: */
typedef enum { FALSE = 0, TRUE = 1 } bool;

/* Definicao de um tipo protocolo: */
typedef enum { TCP = 0, UDP = 1 } proto_t;

/* Definicao da lista de clientes conectados: */
typedef struct cli_state * cli_state_ptr;
struct cli_state {
	cli_state_ptr prev;
	cli_state_ptr next;
	char username[NAMESIZE];
	proto_t protocol;
	int sock;
	char addr[16];
	int port;
};

/* Declaracao da lista de clientes conectados: */
struct conncli {
	unsigned int len;
	cli_state_ptr head;
	cli_state_ptr tail;
} conncli;

bool verbose;

/* Pragmas das funcoes de controle do servico: */
int control(char **msg_out, const char *cli_msg, proto_t proto, int sock, struct sockaddr_in *sa, socklen_t sa_sz);

/* Pragmas das funcoes de estado do servidor: */
void conncliInit(void);
void conncliInsert(const char *username, const proto_t protocol, const int sock, const char addr[16], const int port);
void conncliDelete(cli_state_ptr del);
void conncliFree(void);
cli_state_ptr conncliSearch(const char username[NAMESIZE]);

/* Pragmas das funcoes auxiliares: */
char *strdup(const char *s);
void srvArgs(int argc, char *argv[], int *port);
bool isExit(const char *msg);

/* Pragmas das funcoes wrapper de rede: */
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
void Connect(int fd, const struct sockaddr *sa, socklen_t salen);
void Listen(int fd, int backlog);
ssize_t Recv(int fd, void *ptr, size_t nbytes, int flags);
ssize_t Recvfrom(int fd, void *ptr, size_t nbytes, int flags, struct sockaddr *sa, socklen_t *salenptr);
int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
void Send(int fd, const void *ptr, size_t nbytes, int flags);
void Sendto(int fd, const void *ptr, size_t nbytes, int flags, const struct sockaddr *sa, socklen_t salen);
void Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);
void Shutdown(int fd, int how);
int Socket(int family, int type, int protocol);

#endif
