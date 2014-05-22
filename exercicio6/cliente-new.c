#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 56789
#define MAXLINE 4096

int Socket(int family, int type, int flags) {
  int sockfd;
  if ((sockfd = socket(family, type, flags)) < 0) {
    perror("socket");
    exit(1);
  }
  else
    return sockfd;
}

void Connect(int sockfd, struct sockaddr_in *servaddr) {
  if (connect(sockfd, (struct sockaddr *) servaddr, sizeof(struct sockaddr_in)) < 0) {
    perror("connect");
    exit(1);
  }
}

int main(int argc, char **argv) {
   int    sockfd, n;
   char   recvline[MAXLINE + 1];
   char   error[MAXLINE + 1];
   char   addrbuf[30];
   struct sockaddr_in servaddr;
   struct sockaddr_in conndata;
   socklen_t socklength;

   struct pollfd ufds[2];
   int rv;
   int bytes_read;
   int bytes_sent;
   char buf0[MAXLINE];
   char buf1[MAXLINE];

   if (argc != 2) {
      strcpy(error,"uso: ");
      strcat(error,argv[0]);
      strcat(error," <IPaddress>");
      perror(error);
      exit(1);
   }

   // Criação do socket
   sockfd = Socket(AF_INET, SOCK_STREAM, 0);

   // Seta os campos da struct com os dados para a conexão
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port   = htons(PORT);
   if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
      perror("inet_pton error");
      exit(1);
   }

   // Realiza a conexão
   Connect(sockfd, &servaddr);

   // Dados da conexão para impressão
   socklength = (socklen_t)sizeof(struct sockaddr_in);
   if (getsockname(sockfd, (struct sockaddr *) &conndata, &socklength) == -1) {
     perror("erro no getsockname");
     exit(1);
   }
   if (!inet_ntop(AF_INET, (void *)&conndata.sin_addr, addrbuf, socklength)) {
     perror("erro no inet_ntop");
     exit(1);
   }
   printf("Conectado a %s na porta %d\n", argv[1], PORT);
   printf("Dados da conexão local: IP %s e porta %d\n", addrbuf, ntohs(conndata.sin_port));

	ufds[0].fd = sockfd;
	ufds[0].events = POLLIN | POLLOUT;

	ufds[1].fd = fileno(stdin);
	ufds[1].events = POLLIN;

	bytes_read = 0;
	bytes_sent = 0;

	memset(buf0, 0, sizeof(buf0));
	memset(buf1, 0, sizeof(buf1));

	while (1)
	{
		n = 0;
		rv = poll(ufds, 2, 1000);

		if ((bytes_sent > 0) && (ufds[0].revents & POLLIN))
		{
			if (read(ufds[0].fd, buf0, strlen(buf0)))
			{
				bytes_sent = 0;
				puts(buf0);
				memset(buf0, 0, sizeof(buf0));n = 1;
			}
		}

		if ((bytes_read > 0) && (ufds[0].revents & POLLOUT))
		{
			send(ufds[0].fd, buf1, strlen(buf1), 0);
			bytes_sent = 1;
			bytes_read = 0;
			memset(buf1, 0, sizeof(buf1));n = 1;
		}

		if ((bytes_read == 0) && (ufds[1].revents & POLLIN))
		{
			fgets(buf1, sizeof(buf1), stdin);
			bytes_read = 1;n = 1;
		}

		if (n)
		{
			printf("%d %d\n", bytes_read, bytes_sent);
		}
	}


   /* Recebe os dados e os imprime
   while (1) {
     fgets(recvline, sizeof(recvline) - 1, stdin);
     n = strlen(recvline) + 1;
     write(sockfd, recvline, n);
     read(sockfd, recvline, n);
     puts(recvline);
   }

   if (n < 0) {
      perror("read error");
      exit(1);
   }*/

   exit(0);
}
