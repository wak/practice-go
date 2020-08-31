#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

void passive_close(int sock)
{
	char buf[10];
	int ret;
	
	while ((ret = read(sock, buf, sizeof(buf))) != 0)
	{
		if (ret < 0) {
			perror("read failed");
			break;
		}
	}
	close(sock);
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
		perror("socket");
		return 1;
	}

	client.sin_family = AF_INET;
	client.sin_port = htons(10010);
	client.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock, (struct sockaddr *)&client, sizeof(client))) {
		perror("bind");
		return 1;
	}
	
	if (inet_aton("127.0.0.1", &addr.sin_addr) == 0) {
		fprintf(stderr, "inet_aton() failed.\n");
		return 1;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("connect");
		return 1;
	}
	
	passive_close(sock);
	return 0;
}

int main(int argc, char **argv)
{
	do_connect();

	return 0;
}
