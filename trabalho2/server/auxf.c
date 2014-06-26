/* auxf.c
 *
 * MC833 - Programacao em Redes de Computadores
 * Trabalho 2: Servidor TCP/UDP
 *
 */

#include "server.h"

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
 *          3. Porta da conexao.
 *
 * output : Por parametros.
 */
void srvArgs(int argc, char *argv[], int *port)
{
	if (argc == 1) {
		verbose = FALSE;
		*port = STD_PORT_NUM;
	}
	else if (argc == 2 && strcmp(argv[1], "-v") == 0) {
		verbose = TRUE;
		*port = STD_PORT_NUM;
	}
	else if (argc == 3 && strcmp(argv[1], "-p") == 0) {
		verbose = FALSE;
		*port = atoi(argv[2]);
	}
	else if (argc == 4) {
		if (strcmp(argv[1], "-v") == 0 && strcmp(argv[2], "-p") == 0) {
			verbose = TRUE;
			*port = atoi(argv[3]);
		}
		else if (strcmp(argv[1], "-p") == 0 && strcmp(argv[3], "-v") == 0) {
			verbose = TRUE;
			*port = atoi(argv[2]);
		}
	}
	else {
		perror("Error! Usage: 'server [-p <PORT>'] [-v]");
		exit(EXIT_FAILURE);
	}

	if (1024 > *port || *port > 49151)
		*port = STD_PORT_NUM;
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
