/* ServEco
 *
 * MC833 - Programacao em Redes de Computadores
 * Exercicio 7: Servidor de Eco TCP/UDP
 *
 * Autor: Raul Rabelo Carvalho, 105607
 */

#include "myNetworking.h"

int main(int argc, char *argv[])
{
  /* Variaveis para estabelecimento dos sockets: */
  int port;
  int sock_tcp, sock_udp, sock_tcp_c;
  int optval_tcp = 1, optval_udp = 1;
  struct sockaddr_in srv_tcp, srv_udp, cli_tcp, cli_udp;
  unsigned int cli_tcp_sz, cli_udp_sz;

  /* Variaveis para fork do processo: */
  struct sigaction sa;
  pid_t pid;

  /* Variaveis para selecionar TCP ou UDP: */
  fd_set rfds0, rfds1;
  struct timeval tv;
  int maxfd, rval;

  /* Variaveis gerais: */
  bool isClosing;
  char buf_str[BUFSIZE];
  int  buf_len;

/* ------------------------------------------------------------------------ */

  isClosing = FALSE;

  /* Configura o handler para encerrar os procesos-zumbis: */
  signalHandler(&sa);

  /* Configura a porta a ser usada: */
  port = srvArgs(argc, argv);

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

  Bind(sock_udp, (SA *)&srv_udp, sizeof(srv_tcp));

  /* Configurando o select para os sockets TCP e UDP: */
  maxfd = (sock_tcp > sock_udp) ? sock_tcp : sock_udp;
  FD_ZERO(&rfds0);
  FD_SET(sock_tcp, &rfds0);
  FD_SET(sock_udp, &rfds0);
  tv.tv_sec = 0;
  tv.tv_usec = 0;

  memset(buf_str, 0, sizeof(buf_str));

  while (1)
  {
    rfds1 = rfds0;

    /* Verifica se ha conexoes TCP ou mensagens UDP: */
    rval = Select(maxfd + 1, &rfds1, NULL, NULL, &tv);

    if (rval > 0 && FD_ISSET(sock_tcp, &rfds1))
    /* Caso TCP: */
    {
      rval--;

      /* Aceita a conexao com o cliente TCP: */
      cli_tcp_sz = sizeof(cli_tcp);
      sock_tcp_c = Accept(sock_tcp, (SA *)&cli_tcp, &cli_tcp_sz);

      /* Cria um processo filho para atender ao cliente TCP: */
      pid = Fork();

      if (pid == 0)
      /* Caso seja o processo-filho: */
      {
        /* Fecha a conexao de escuta: */
        close(sock_tcp);

        /* Loop sobre as mensagens recebidas: */
        while ((buf_len = Recv(sock_tcp_c, buf_str, sizeof(buf_str), 0)) > 0)
        {
          /* Encerra a conexao: */
          if ((isClosing = isExit(buf_str)))
            break;

          /* Envia o eco. */
          Send(sock_tcp_c, buf_str, buf_len, 0);

          memset(buf_str, 0, sizeof(buf_str));
        }

        /* Fecha a conexao com o cliente: */
        close(sock_tcp_c);
        isClosing = TRUE;
      }
      else
      /* Caso seja o processo-pai: */
      {
        /* Encerra a conexao com o cliente TCP: */
        close(sock_tcp_c);
      }
    }

    if (rval > 0 && FD_ISSET(sock_udp, &rfds1))
    /* Caso UDP: */
    {
      rval--;

      /* Recebe mensagem UPD: */
      cli_udp_sz = sizeof(cli_udp);
      buf_len = Recvfrom(sock_udp, buf_str, sizeof(buf_str), 0, (SA *)&cli_udp, &cli_udp_sz);

      /* Envia o eco: */
      Sendto(sock_udp, buf_str, strlen(buf_str), 0, (SA *)&cli_udp, cli_udp_sz);

      memset(buf_str, 0, sizeof(buf_str));
    }

    /* Encerra o servidor-filho a pedido do cliente TCP: */
    if (isClosing) break;
  }

  close(sock_udp);

  return 0;
}
