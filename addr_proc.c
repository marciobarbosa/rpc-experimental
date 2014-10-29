/* Developed by Marcio Brito Barbosa */

#include <stdio.h>
#include <rpc/rpc.h>
#include <memory.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "addr.h"

void
deserialize(struct address *addrs, struct sockaddr *saddr)
{
	struct sockaddr_in *saddr4;
	struct sockaddr_in6 *saddr6;
	int i;

	if (addrs->flag == AF_INET) {
		saddr4 = (struct sockaddr_in *)saddr;
		saddr4->sin_family = addrs->flag;
		saddr4->sin_addr.s_addr = addrs->addr_1;
	} else {
		saddr6 = (struct sockaddr_in6 *)saddr;
		saddr6->sin6_family = addrs->flag;

		for (i = 0; i < 4; i++) {
			saddr6->sin6_addr.s6_addr[i] = (char)(addrs->addr_1 >> ((3 - i) * 8));
		}

		for (; i < 8; i++) {
			saddr6->sin6_addr.s6_addr[i] = (char)(addrs->addr_2 >> ((7 - i) * 8));
		}

		for (; i < 12; i++) {
			saddr6->sin6_addr.s6_addr[i] = (char)(addrs->addr_3 >> ((11 - i) * 8));
		}

		for (; i < 16; i++) {
			saddr6->sin6_addr.s6_addr[i] = (char)(addrs->addr_4 >> ((15 - i) * 8));
		}
	}
}

int *
printaddress_1_svc(struct address *addr, struct svc_req *req)
{
	static int result = 1;
	struct sockaddr_in saddr4;
	struct sockaddr_in6 saddr6;
	char str[INET6_ADDRSTRLEN];

	memset(&saddr4, 0, sizeof(struct sockaddr_in));
	memset(&saddr6, 0, sizeof(struct sockaddr_in6));
	memset(str, 0, INET6_ADDRSTRLEN);

	if (addr->flag == AF_INET) {
		deserialize(addr, (struct sockaddr *)&saddr4);
		inet_ntop(AF_INET, &(saddr4.sin_addr), str, INET_ADDRSTRLEN);
		printf("Final address: %s\n", str);
	} else {
		deserialize(addr, (struct sockaddr *)&saddr6);
		inet_ntop(AF_INET6, &(saddr6.sin6_addr), str, INET6_ADDRSTRLEN);
		printf("Final address: %s\n", str);
	}

	return (&result);
}