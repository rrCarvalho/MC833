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

/* Tamanho maximo dos buffers: */
#define MAXLINE 4096
#define BUFSIZE 8192

/* Porta de comunicacao padrao: */
#define STD_PORT_NUM  49151
#define STD_PORT_STR  "49151"

#define BACKLOG 1024

#define SA  struct sockaddr

/* Macros para o segundo parametro de Shutdown: */
#define SHUTRECV 0 /* further receives are disallowed */
#define SHUTSEND 1 /* further sends are disallowed */
#define SHUTBOTH 2 /* further sends and receives are disallowed */

#ifndef MYNETWORKING_H_INCLUDED
#define MYNETWORKING_H_INCLUDED

/* Definicao de um tipo booleano: */
typedef enum { FALSE = 0, TRUE = 1 } bool;

/* Definicao de um tipo protocolo: */
typedef enum { TCP = 0, UDP = 1 } proto_t;

/* Pragmas das funcoes auxiliares: */
int srvArgs(int argc, char *argv[]);
proto_t cliArgs(char **addr, int *port, int argc, char *argv[]);
void signalHandler(struct sigaction *sa);
pid_t Fork();
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
int Socket(int family, int type, int protocol);

#endif


