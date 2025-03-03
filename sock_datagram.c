#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "sock_datagram_un"

static void print_usage(const char *progname)
{
	printf("%s (server|client)\n", progname);
}

// socket() -> bind() -> recvfrom() -> close()
static int do_server(void)
{
	int sock;
	struct sockaddr_un addr;
	char buf[128];
	int ret;

	// socket()
	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock == -1)
	{
		perror("socket()");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path)-1);

	// bind()
	if (bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
	{
		perror("bind()");
		close(sock);
		return -1;	
	}	

	// recvfrom()
	memset(buf, 0, sizeof(buf));
	ret = recvfrom(sock, buf, sizeof(buf), 0, NULL, NULL);
	if (ret == -1)
	{
		perror("recvfrom()");
		close(sock);
		return -1;
	}
	printf("Client said [%s]\n", buf);

	// close()
	close(sock);


	return 0;	
}

// socket() -> sendto() -> close()
static int do_client(void)
{
	int sock;
	struct sockaddr_un addr;
	char buf[128];
	int ret;

	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		perror("socket()");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path)-1);

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "this is msg from sock_datagram");

	ret = sendto(sock, buf, sizeof(buf), 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
	if (ret < 0)
	{
		perror("sendto()");
		close(sock);
		return -1;
	}

	close(sock);

	return 0;
}

int main(int argc, char **argv)
{
	int ret;

	if (argc < 2)
	{
		print_usage(argv[0]);
		return -1;
	}

	if (!strcmp(argv[1], "server"))
	{
		ret = do_server();
	}
	else if (!strcmp(argv[1], "client"))
	{
		ret = do_client();
	}
	else 
	{
		print_usage(argv[0]);
		return -1;
	}

	return ret;
}

