#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER_PORT 10101
#define MAX_PENDING 5
#define MAX_LINE 256

int main()
{
	struct sockaddr_in sin;
	char buf[MAX_LINE];
	int len;
	int s, new_s;
	/* build address data structure */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(SERVER_PORT);

	struct sockaddr_in so;
	int so_len;
	char so_addr[INET_ADDRSTRLEN];

	/* setup passive open */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("simplex-talk: socket");
		exit(1);
	}

if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
		perror("simplex-talk: bind");
		exit(1);
	}
	listen(s, MAX_PENDING);
	/* wait for connection, then receive and print text */
	while(1) {
		if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) {
			perror("simplex-talk: accept");
			exit(1);
		}
		/* get socket information and prints it on the stdout */
		so_len = sizeof(so);
		if (getpeername(new_s, (struct sockaddr *)&so, &so_len) < 0) {
			perror("simplex-talk: getpeername");
			close(s);
			exit(1);
		}
		inet_ntop(AF_INET, &(so.sin_addr), so_addr, INET_ADDRSTRLEN);
		printf("IP address: %s; Port number: %d\n", so_addr, ntohs(so.sin_port));
		while (len = recv(new_s, buf, sizeof(buf), 0)) {
			fputs(buf, stdout);
		}
		close(new_s);
	}
}
