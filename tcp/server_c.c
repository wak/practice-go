#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>

static time_t beg0;
static time_t beg_base;
static int nr_connections = 0;
static int nr_base = 0;

void *measure(void *arg)
{
    pthread_detach(pthread_self());

	while (1) {
		time_t now = 0;
		sleep(5);
		now = time(NULL);
		fprintf(stdout, "%10d  %5d/s, %5d/s\n",
				nr_connections,
				(int)(nr_connections / (now - beg0)),
				(int)((nr_connections - nr_base) / (now - beg_base)));
		time(&beg_base);
		nr_base = nr_connections;
	}
    return 0;
}

void active_close(int sock)
{
	close(sock);
}

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

int main(int argc, char **argv)
{
	int sock0;
	
	struct sockaddr_in addr;
	struct sockaddr_in client;
	int sock;
	pthread_t pt;

	beg0 = beg_base = time(NULL);
	pthread_create(&pt, NULL, &measure, NULL);
	
	sock0 = socket(AF_INET, SOCK_STREAM, 0);
	if (sock0 < 0) {
		perror("socket failed");
		return 1;
	}
	
	/* int reuseaddr = 1; */
	/* if (setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (const void *) &reuseaddr, sizeof(int)) < 0) { */
	/* 	perror("setsockopt failed"); */
	/* 	return 1; */
	/* } */

	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(sock0, (struct sockaddr *)&addr, sizeof(addr))) {
		perror("bind failed");
		return 1;
	}
	if (listen(sock0, 10) < 0) {
		perror("listen failed");
		return 1;
	}

	printf("START\n");
	while (1) {
		socklen_t len = sizeof(client);
		sock = accept(sock0, (struct sockaddr *)&client, &len);
		if (sock < 0) {
			perror("error");
			continue;
		}
		nr_connections++;
		/* passive_close(sock); */
		active_close(sock);
	}

	close(sock0);
	return 0;
}
