/* AuxF
 *
 * MC833 - Programacao em Redes de Computadores
 * Exercicio 7: Servidor de Eco TCP/UDP
 *
 * Autor: Raul Rabelo Carvalho, 105607
 */

#include "myNetworking.h"

/* ------------------------------------------------------------------------ */

char *strdup(const char *s)
{
  size_t len = 1 + strlen(s);
  char *p = malloc(len);
  return p ? memcpy(p, s, len) : NULL;
}

/* --- srvArgs() ----------------------------------------------------------
 *
 * desc   : Verifica e processa os argumentos do servidor.
 *
 * params : 1. Numero de argumentos passados.
 *          2. Vetor com os argumentos.
 *
 * output : Inteiro com o numero da porta de conexao do servidor.
 */
int srvArgs(int argc, char *argv[])
{
  int  port;

  if (argc != 1 && argc != 2)
  {
    perror("Error! Usage: 'servidor <PORT>' ou 'servidor'");
    exit(EXIT_FAILURE);
  }

  port = argc > 1 ? atoi(argv[1]) : 0;

  if (1024 > port || port > 49151)
    port = STD_PORT_NUM;

  return port;
}

/* --- cliArgs() ----------------------------------------------------------
 *
 * desc   : Verifica e processa os argumentos do cliente.
 *
 * params : 1. (Saida da funcao.) Ponteiro para string com o endereco IP.
 *          2. (Saida da funcao.) Ponteiro para o inteiro com a porta.
 *          3. Numero de argumentos passados.
 *          4. Vetor com os argumentos.
 *
 * output : Tipo proto_t indica qual protocolo deve ser usado:
 *          TCP equivale ao inteiro 0 e UDP ao inteiro 1.
 */
proto_t cliArgs(char **addr, int *port, int argc, char *argv[])
{
  proto_t proto;
  char *c;

  if (argc != 3 && argc != 4)
  {
    perror("Error! Usage: 'cliente <PROTOCOL> <IP_ADDR> <PORT>'");
    exit(EXIT_FAILURE);
  }

  c = argv[1];
  for ( ; *c; ++c) *c = tolower(*c);

  /* Seleciona o protocolo: */
  if (strncmp(argv[1], "tcp", 3) == 0)
    proto = TCP;
  else if (strncmp(argv[1], "udp", 3) == 0)
    proto = UDP;
  else
  {
    perror("Error! The protocol must be either TCP or UDP");
    exit(EXIT_FAILURE);
  }

  /* Retora endereco IP: */
  *addr = strdup(argv[2]);

  /* Seleciona a porta: */
  *port = argc > 3 ? atoi(argv[3]) : 0;
  if (1024 > *port || *port > 49151)
    *port = STD_PORT_NUM;

  return proto;
}

/* --- signalHandler() ----------------------------------------------------
 *
 * desc   :  Configura o processo para ser encerrado caso fique inativo.
 *
 * params : 1. Estrutura na qual se configura como sinais do UNIX serão
 *             tratados.
 *
 * output : Nenhuma.
 *
 */
void signalHandler(struct sigaction *sa)
{
  sa->sa_handler = SIG_DFL;
  sigemptyset(&sa->sa_mask);
  sa->sa_flags = SA_NOCLDWAIT;
  if (sigaction(SIGCHLD, sa, NULL) == -1) {
    perror("sigaction error");
    exit(1);
  }
}

/* --- Fork() -------------------------------------------------------------
 *
 * desc   : Cria uma copia do processo corrente.
 *
 * params : Nenhum.
 *
 * output : Identificador do processo-filho.
 */
pid_t Fork()
{
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  return pid;
}

/* --- isExit() -----------------------------------------------------------
 *
 * desc   : Verifica se uma string contem a palavra 'exit'.
 *
 * params : 1. String a ser verificada.
 *
 * output : Tipo bool: TRUE ou FALSE.
 */
bool isExit(const char *msg)
{
  bool r = FALSE;
  int len = strlen(msg);

  if (strncmp(msg, "exit\n", len) == 0 ||
      strncmp(msg, "exit\r", len) == 0 ||
      strncmp(msg, "exit\r\n", len) == 0)
    r = TRUE;

  return r;
}
