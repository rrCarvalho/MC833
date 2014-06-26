/* control.c
 *
 * MC833 - Programacao em Redes de Computadores
 * Trabalho 2: Servidor TCP/UDP
 *
 */

#include "server.h"

int control(char **msg_out, const char *cli_msg, proto_t proto, int sock, struct sockaddr_in *sa, socklen_t sa_sz)
{
	/* Retorno do controle: */
	int rval = 0;

	/* Variaveis gerais: */
	cli_state_ptr cli_tmp;
	char *input = strdup(cli_msg);
	char *tok;
	char msg[BUFSIZE];
	char cmd[64];
	char sender[NAMESIZE];


	if ((tok = strtok(input, ":\n\r")) != NULL) {
		strncpy(cmd, tok, 64);
		if ((tok = strtok(NULL, ":\n\r")) != NULL) {
			strncpy(sender, tok, NAMESIZE);

			if (strcmp(cmd, "talk") == 0) {
				*msg_out = strdup(cli_msg);
				rval = 2;
			}

			/* Cliente conectando ao servico: */
			else if (strcmp(cmd, "join") == 0) {
				if ((cli_tmp = conncliSearch(sender)) == NULL) {
					conncliInsert(sender, proto, sock, inet_ntoa(sa->sin_addr), ntohs(sa->sin_port));
					memset(msg, 0, sizeof(msg));
					sprintf(msg, "joined:%s\n", sender);
					*msg_out = strdup(msg);
					rval = 1;
					if (verbose) {
						printf("SRV_LOG: User %s at %s:%d joined the service.\n", sender, inet_ntoa(sa->sin_addr), ntohs(sa->sin_port));
					}
				}
				else {
					memset(msg, 0, sizeof(msg));
					sprintf(msg, "rejected:%s\n", sender);
					*msg_out = strdup(msg);
					rval = 1;
					if (verbose) {
						printf("SRV_LOG: User %s already in the service.\n", sender);
					}
				}

			}

			/* Cliente deixando o servico: */
			else if (strcmp(cmd, "leave") == 0) {
				cli_tmp = conncliSearch(sender);
				if (cli_tmp != NULL) {
					conncliDelete(cli_tmp);
					memset(msg, 0, sizeof(msg));
					sprintf(msg, "left:%s\n", tok);
					*msg_out = strdup(msg);
					rval = 3;
					if (verbose) {
						printf("SRV_LOG: User %s at %s:%d left the service.\n", sender, inet_ntoa(sa->sin_addr), ntohs(sa->sin_port));
					}
				}
			}

			/* Verificando se um cliente esta online: */
			else if (strcmp(cmd, "query") == 0) {
				if ((tok = strtok(NULL, "\n\r")) != NULL) {
					if ((cli_tmp = conncliSearch(tok)) != NULL) {
						memset(msg, 0, sizeof(msg));
						sprintf(msg, "online:%s\n", cli_tmp->username);
						*msg_out = strdup(msg);
						rval = 1;
					}
					else {
						memset(msg, 0, sizeof(msg));
						sprintf(msg, "offline:%s\n", tok);
						*msg_out = strdup(msg);
						rval = 1;
					}
					if (verbose) {
						printf("SRV_LOG: User %s queried about %s.\n", sender, tok);
					}
				}
			}
		}
	}

	return rval;
}

