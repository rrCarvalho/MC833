/* state.c
 *
 * MC833 - Programacao em Redes de Computadores
 * Trabalho 2: Servidor TCP/UDP
 *
 */

#include "server.h"

void conncliInit(void)
{
	conncli.len = 0;
	conncli.head = NULL;
	conncli.tail = NULL;
}

void conncliInsert(
		const char *username,
		const proto_t protocol,
		const int sock,
		const char addr[16],
		const int port)
{
	cli_state_ptr ins = malloc(sizeof(cli_state_ptr));

	strcpy(ins->username, username);
	ins->protocol = protocol;
	ins->sock = sock;
	strcpy(ins->addr, addr);
	ins->port = port;

	if (conncli.len == 0) {
		ins->prev = ins;
		ins->next = ins;
		conncli.head = ins;
		conncli.tail = ins;
		conncli.len = 1;
	}
	else {
		ins->prev = conncli.tail;
		ins->next = conncli.head;
		conncli.head->prev = ins;
		conncli.tail->next = ins;
		conncli.tail = ins;
		conncli.len++;
	}
}

void conncliDelete(cli_state_ptr del)
{
	if (conncli.len == 1) {
		conncli.len = 0;
		conncli.head = NULL;
		conncli.tail = NULL;
	}
	else if (conncli.len > 1) {
		conncli.len--;

		if (conncli.head == del) {
			conncli.head = del->next;
		}
		else if (conncli.tail == del) {
			conncli.tail = del->prev;
		}

		del->prev->next = del->next;
		del->next->prev = del->prev;
	}

	free(del);
}

void conncliFree(void)
{
	while (conncli.len > 0) {
		conncliDelete(conncli.tail);
	}
}

cli_state_ptr conncliSearch(const char username[NAMESIZE])
{
	bool is_present = FALSE;
	cli_state_ptr tmp = NULL;
	int i;

	if (conncli.len > 0) {
		tmp = conncli.head;

		for (i = 0; i < conncli.len; i++) {

			if (strcmp(tmp->username, username) == 0) {
				is_present = TRUE;
				break;
			}
			tmp = tmp->next;
		}
	}

	return (is_present) ? tmp : NULL;
}

void conncliPrintAll(void)
{
	cli_state_ptr tmp = NULL;
	int i;

	if (conncli.len > 0) {
		tmp = conncli.head;
		for (i = 0; i < conncli.len; i++) {
			puts(tmp->username);
			if (tmp->protocol == TCP) puts("TCP");
			else if (tmp->protocol == UDP) puts("UDP");
			else puts("wtp!?");
			printf("%d %s %d\n\n", (int)strlen(tmp->username), tmp->addr, tmp->port);
			tmp = tmp->next;
		}
	} else puts("conncli is empty");
}
