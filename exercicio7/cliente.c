/* Cliente
 *
 * MC833 - Programacao em Redes de Computadores
 * Exercicio 7: Servidor de Eco TCP/UDP
 *
 * Autor: Raul Rabelo Carvalho, 105607
 */

#include "myNetworking.h"

int main(int argc, char *argv[])
{
  /* Variaveis com os dados do servidor: */
  proto_t proto;
  char *addr;
  int port;

  /* Variaveis para conexao de rede: */
  int sock;
  int optval = 1;
  struct sockaddr_in srv_addr;

  /* Variaveis gerais: */
  char line[MAXLINE];
  char buf_str[BUFSIZE];
  int  buf_len;

/* ------------------------------------------------------------------------ */

  /* Coleta os dados do servidor: */
  proto = cliArgs(&addr, &port, argc, argv);

  /* Cria o socket com o protocolo escolhido: */
  if (proto == TCP)
    sock = Socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  else if (proto == UDP)
    sock = Socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  /* Configura o socket para reutilizar o mesmo endereco IP: */
  Setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

  /* Configura os dados do servidor: */
  memset(&srv_addr, 0, sizeof(srv_addr));
  srv_addr.sin_addr.s_addr = inet_addr(addr);
  srv_addr.sin_port        = htons(port);
  srv_addr.sin_family      = PF_INET;

  /* Conecta ao servidor: */
  Connect(sock, (SA *)&srv_addr, sizeof(srv_addr));

  /* Loop sobre a entrada de mensagens: */
  while (fgets(line, MAXLINE, stdin) != NULL)
  {
    /* Envia a mensagem ao servidor: */
    Send(sock, line, strlen(line), 0);

    if (isExit(line)) break;

    /* Recebe o eco do servidor: */
    buf_len = Recv(sock, buf_str, BUFSIZE, 0);

    /* Imprime o eco: */
    buf_str[buf_len++] = 0;
    printf("%s", buf_str);
  }

  return 0;
}
