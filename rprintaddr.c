/* Developed by Marcio Brito Barbosa */

#include <stdio.h>
#include <rpc/rpc.h>
#include <memory.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "addr.h"

void
serialize(struct sockaddr *saddr, struct address *addrs)
{
	struct sockaddr_in *saddr4;
	struct sockaddr_in6 *saddr6;
	int i, aux;

	if (saddr->sa_family == AF_INET) {
		saddr4 = (struct sockaddr_in *)saddr;
		addrs->flag = AF_INET;
		addrs->addr_1 = saddr4->sin_addr.s_addr;
		addrs->addr_2 = 0;
		addrs->addr_3 = 0;
		addrs->addr_4 = 0;
	} else {
		saddr6 = (struct sockaddr_in6 *)saddr;
		addrs->flag = AF_INET6;

		memset(&addrs->addr_1, 0, sizeof(unsigned int));
		memset(&addrs->addr_2, 0, sizeof(unsigned int));
		memset(&addrs->addr_3, 0, sizeof(unsigned int));
		memset(&addrs->addr_4, 0, sizeof(unsigned int));

		for (i = 0; i < 4; i++) {
			memset(&aux, 0, sizeof(int));
			aux = (int)saddr6->sin6_addr.s6_addr[i];
			aux = aux << (3 - i) * 8;
			addrs->addr_1 = addrs->addr_1 | aux;
		}

		for (; i < 8; i++) {
			memset(&aux, 0, sizeof(int));
			aux = (int)saddr6->sin6_addr.s6_addr[i];
			aux = aux << (7 - i) * 8;
			addrs->addr_2 = addrs->addr_2 | aux;
		}

		for (; i < 12; i++) {
			memset(&aux, 0, sizeof(int));
			aux = (int)saddr6->sin6_addr.s6_addr[i];
			aux = aux << (11 - i) * 8;
			addrs->addr_3 = addrs->addr_3 | aux;
		}

		for (; i < 16; i++) {
			memset(&aux, 0, sizeof(int));
			aux = (int)saddr6->sin6_addr.s6_addr[i];
			aux = aux << (15 - i) * 8;
			addrs->addr_4 = addrs->addr_4 | aux;
		}
	}
}

int
main(int argc, char *argv[])
{
	CLIENT *cl;
	int *result;
	char *server;
	struct address addrs;
	struct sockaddr_in saddr4;
	struct sockaddr_in6 saddr6;

	if (argc < 2) {
		fprintf(stderr, "usage: %s host\n", argv[0]);
		exit(1);
	}

	server = argv[1];

	memset(&saddr4, 0, sizeof(struct sockaddr_in));
	memset(&saddr6, 0, sizeof(struct sockaddr_in6));
	memset(&addrs, 0, sizeof(struct address));

	saddr4.sin_family = AF_INET;
	inet_pton(AF_INET, "192.0.2.33", &(saddr4.sin_addr));

	saddr6.sin6_family = AF_INET6;
	inet_pton(AF_INET6, "2001:db8:8714:3a90::12", &(saddr6.sin6_addr));

	/* serialize((struct sockaddr *)&saddr4, &addrs); */
	serialize((struct sockaddr *)&saddr6, &addrs);

	cl = clnt_create(server, ADDRESSPROG, ADDRESSVERS, "tcp");

	if (cl == NULL) {
		clnt_pcreateerror(server);
		exit(1);
	}

	result = printaddress_1(&addrs, cl);

	if (result == NULL) {
		clnt_perror(cl, server);
		exit(1);
	}

	if(*result == 0) {
		fprintf(stderr, "%s: %s couldn't print your message\n", argv[0], server);
		exit(1);
	}

	printf("Message delivered to %s!\n", server);

	return EXIT_SUCCESS;
}