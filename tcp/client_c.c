#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

static char *target_ip_address = "127.0.0.1";

#define BINDPORT 0

void passive_close(int sock)
{
	char buf[10];
	int ret;
	int count = 0;
	
	while ((ret = read(sock, buf, sizeof(buf))) != 0)
	{
		if (ret < 0) {
			perror("read()");
			break;
		}
		count += ret;
	}
	if (close(sock) < 0) {
		perror("close()");
		exit(1);
	}

	printf("%d bytes read.\n", count);
}

void active_close(int sock)
{
	close(sock);
}

int do_connect(int count)
{
	struct sockaddr_in addr;
	struct sockaddr_in client;
	int sock;

	memset(&addr, 0, sizeof(struct sockaddr_in));
	memset(&client, 0, sizeof(struct sockaddr_in));
	
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		perror("socket()");
		return 1;
	}

#if BINDPORT
	client.sin_family = AF_INET;
	client.sin_port = htons(10010);
	client.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock, (struct sockaddr *)&client, sizeof(client))) {
		perror("bind()");
		close(sock);
		return 1;
	}
#endif

	if (inet_aton(target_ip_address, &addr.sin_addr) == 0) {
		fprintf(stderr, "inet_aton() failed.\n");
		close(sock);
		return 1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("connect()");
		close(sock);
		return 1;
	}

	struct sockaddr_in addr2;
	memset(&addr2, 0, sizeof(struct sockaddr_in));
	socklen_t addr2_len = sizeof(addr2);
	if (getsockname(sock, (struct sockaddr *) &addr2, &addr2_len) < 0) {
		perror("getsockname()");
		close(sock);
		return 1;
	}
	
	printf("[%6d] :%-5d -> %s:8080\n", count, ntohs(addr2.sin_port), target_ip_address);
	active_close(sock);
	return 0;
}

int main(int argc, char **argv)
{
	int i;

	if (argc > 1)
		target_ip_address = argv[1];

	for (i = 0; i < 10000000; i++) {
		do_connect(i);
		/* sleep(0.5); */
	}

	return 0;
}
