/* server.c
 *
 * MC833 - Programacao em Redes de Computadores
 * Trabalho 2: Servidor TCP/UDP
 *
 */

#include "server.h"

int main(int argc, char *argv[])
{
	/* Variaveis para estabelecimento dos sockets: */
	int port;
	int sock_tcp, sock_udp, sock_tcp_c;
	int optval_tcp = 1, optval_udp = 1;
	struct sockaddr_in srv_tcp, srv_udp, cli_tcp, cli_udp;
	unsigned int cli_tcp_sz, cli_udp_sz;

	/* Variaveis para selecionar TCP ou UDP: */
	fd_set rfds0, rfds1;
	struct timeval tv;
	int sockn;

	/* Variaveis gerais: */
	char buf_str[BUFSIZE];
	char *msg_out;
	int rval;
	int i;

	/* Variaveis de estado do servico: */
	cli_state_ptr cli_tmp;

/* ------------------------------------------------------------------------ */

	/* Inicia a lista de clientes conectados: */
	conncliInit();

	/* Obtem os argumentos do programa: */
	srvArgs(argc, argv, &port);

	/* Cria e configura o socket TCP: */
	sock_tcp = Socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	Setsockopt(sock_tcp, SOL_SOCKET, SO_REUSEADDR, &optval_tcp, sizeof(int));

	memset(&srv_tcp, 0, sizeof(srv_tcp));
	srv_tcp.sin_addr.s_addr = INADDR_ANY;
	srv_tcp.sin_port        = htons(port);
	srv_tcp.sin_family      = PF_INET;

	Bind(sock_tcp, (SA *)&srv_tcp, sizeof(srv_tcp));

	Listen(sock_tcp, BACKLOG);

	/* Cria e configura o socket UDP: */
	sock_udp = Socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	Setsockopt(sock_udp, SOL_SOCKET, SO_REUSEADDR, &optval_udp, sizeof(int));

	memset(&srv_udp, 0, sizeof(srv_udp));
	srv_udp.sin_addr.s_addr = INADDR_ANY;
	srv_udp.sin_port        = htons(port);
	srv_udp.sin_family      = PF_INET;

	Bind(sock_udp, (SA *)&srv_udp, sizeof(srv_udp));

	if (verbose) {
		printf("SRV_LOG: Listening on TCP and UDP ports %d.\n", port);
	}

	/* Configurando o select para os sockets TCP e UDP: */
	FD_ZERO(&rfds0);
	FD_SET(sock_tcp, &rfds0);
	FD_SET(sock_udp, &rfds0);
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	memset(buf_str, 0, sizeof(buf_str));

	while (1) {
		rfds1 = rfds0;

		/* Verifica se ha conexoes TCP ou mensagens UDP: */
		sockn = Select(FD_SETSIZE, &rfds1, NULL, NULL, &tv);

		/* Caso UDP: */
		if (FD_ISSET(sock_udp, &rfds1)) {
			sockn--;

			/* Recebe mensagem UPD: */
			cli_udp_sz = sizeof(cli_udp);
			Recvfrom(sock_udp, buf_str, sizeof(buf_str), 0, (SA *)&cli_udp, &cli_udp_sz);

			if (verbose) {
				printf("MSG_LOG: Received UDP message from %s at port %d: %s\n", inet_ntoa(cli_udp.sin_addr), ntohs(cli_udp.sin_port), buf_str);
			}

			/* Envia a mensagem para a funcao de controle do servico: */
			rval = control(&msg_out, buf_str, UDP, -1, (struct sockaddr_in *)&cli_udp, cli_udp_sz);

			/* Envia mensagem de retorno ao cliente: */
			if (rval == 1 || rval == 3) {
				Sendto(sock_udp, msg_out, strlen(msg_out), 0, (SA *)&cli_udp, cli_udp_sz);
				free(msg_out);
			}
			/* Envia multicast aos clientes: */
			else if (rval == 2) {
				if (conncli.len > 0) {
					cli_tmp = conncli.head;
					for (i = 0; i < conncli.len; i++) {
						/* Para clientes TCP: */
						if (cli_tmp->protocol == TCP) {
							Send(cli_tmp->sock, buf_str, strlen(buf_str), 0);
							if (verbose) {
								printf("MSG_LOG: Sent message to %s at %s:%d: %s\n", cli_tmp->username, cli_tmp->addr, cli_tmp->port, msg_out);
							}
						}
						/* Para clientes UDP: */
						else if (cli_tmp->protocol == UDP) {
							memset(&cli_udp, 0, sizeof(cli_udp));
							cli_udp.sin_addr.s_addr = inet_addr(cli_tmp->addr);
							cli_udp.sin_port        = htons(cli_tmp->port);
							cli_udp.sin_family      = PF_INET;
							Sendto(sock_udp, msg_out, strlen(msg_out), 0, (SA *)&cli_udp, cli_udp_sz);
						}
						cli_tmp = cli_tmp->next;
					}
				}
				free(msg_out);
			}
			memset(buf_str, 0, sizeof(buf_str));
		}

		/* Caso TCP: */
		if (FD_ISSET(sock_tcp, &rfds1)) {
			sockn--;

			/* Aceita a conexao com o cliente TCP: */
			cli_tcp_sz = sizeof(cli_tcp);
			sock_tcp_c = Accept(sock_tcp, (SA *)&cli_tcp, &cli_tcp_sz);
			FD_SET(sock_tcp_c, &rfds0);
		}

		/* Percorre clientes TCP conectados: */
		for (sock_tcp_c = 0; sockn; sock_tcp_c++) {

			if (FD_ISSET(sock_tcp_c, &rfds1)) {
				sockn--;

				cli_tcp_sz = sizeof(cli_tcp);
				Recvfrom(sock_tcp_c, buf_str, sizeof(buf_str), 0, (SA *)&cli_tcp, &cli_tcp_sz);

				if (verbose) {
					printf("MSG_LOG: Received TCP message from %s at port %d: %s\n", inet_ntoa(cli_tcp.sin_addr), ntohs(cli_tcp.sin_port), buf_str);
				}

				rval = control(&msg_out, buf_str, TCP, sock_tcp_c, (struct sockaddr_in *)&cli_tcp, cli_tcp_sz);

				/* Envia resposta ao cliente: */
				if (rval == 1) {
					Send(sock_tcp_c, msg_out, strlen(msg_out), 0);
					free(msg_out);
				}

				/* Envia multicast aos clientes: */
				else if (rval == 2) {
					if (conncli.len > 0) {
						cli_tmp = conncli.head;
						for (i = 0; i < conncli.len; i++) {
							/* Para clientes TCP: */
							if (cli_tmp->protocol == TCP) {
								Send(cli_tmp->sock, buf_str, strlen(buf_str), 0);
								if (verbose) {
									printf("MSG_LOG: Sent message to %s at %s:%d: %s\n", cli_tmp->username, cli_tmp->addr, cli_tmp->port, msg_out);
								}
							}
							/* Para clientes UDP: */
							else if (cli_tmp->protocol == UDP) {
								memset(&cli_udp, 0, sizeof(cli_udp));
								cli_udp.sin_addr.s_addr = inet_addr(cli_tmp->addr);
								cli_udp.sin_port        = htons(cli_tmp->port);
								cli_udp.sin_family      = PF_INET;
								Sendto(sock_udp, msg_out, strlen(msg_out), 0, (SA *)&cli_udp, cli_udp_sz);
							}
							cli_tmp = cli_tmp->next;
						}
					}
					free(msg_out);
				}

				/* Envia resposta de encerramento ao cliente e o retira da lista: */
				else if (rval == 3) {
					Send(sock_tcp_c, msg_out, strlen(msg_out), 0);
					Shutdown(sock_tcp_c, SHUTBOTH);
					FD_CLR(sock_tcp_c, &rfds0);
					free(msg_out);
				}

				memset(buf_str, 0, sizeof(buf_str));
			}

		}/* for (i = 0; sockn; i++) closure */

	}/* while(1) closure */

	return 0;
}
