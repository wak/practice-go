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

int do_connect(void)
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

	client.sin_family = AF_INET;
	client.sin_port = htons(10010);
	client.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock, (struct sockaddr *)&client, sizeof(client))) {
		perror("bind()");
		close(sock);
		return 1;
	}

	printf("target: %s\n", target_ip_address);
	if (inet_aton(target_ip_address, &addr.sin_addr) == 0) {
		fprintf(stderr, "inet_aton() failed.\n");
		close(sock);
		return 1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	/* addr.sin_addr.s_addr = INADDR_ANY; */
	if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("connect()");
		close(sock);
		return 1;
	}

	passive_close(sock);
	return 0;
}

int main(int argc, char **argv)
{
	int i;

	if (argc > 1)
		target_ip_address = argv[1];

	for (i = 0; i < 100; i++) {
		do_connect();
		sleep(0.5);
	}

	return 0;
}
