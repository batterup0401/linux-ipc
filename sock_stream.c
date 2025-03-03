#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "sock_stream_un"

static void print_usage(const char *progname)
{
	printf("%s (server|client)\n", progname);
}

int stream_send(int sock, void *buf, size_t buflen, int flag)
{
	int written = 0;
	int ret;

	while(written < buflen)
	{
		ret = send(sock, (char *)buf + written, buflen - written, flag);
		
		if (ret == -1)
		{
			return ret;
		}

		written += ret;
	}

	return 0;
}

int stream_recv(int sock, void *buf, size_t buflen, int flag)
{
	int written = 0;
	int ret;

	while(written < buflen)
	{
		ret = recv(sock, (char *)buf + written, buflen - written, flag);
		
		if (ret == -1)
		{
			return ret;
		}

		written += ret;
	}

	return 0;
}

// socket() -> bind() -> listen() -> accept() -> recv() -> close()
static int do_server(void)
{
	int sock;
	struct sockaddr_un addr; // UNIX Domain Socket
	int peer;
	char buf[128];
	int turn = 0;
	int ret;

	// socket()
	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock == -1)
	{
		perror("socket()");
		return -1;
	}
		
	// Setting socket address(UNIX)
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path)-1);

	// bind(): if applying socket exists, return error
	if (bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
	{
		perror("bind()");
		close(sock);
		return -1;
	}
	
	// listen()
	listen(sock, 5);

	while(1)
	{
		printf("<----- turn: #%d ----->\n", turn++);
	
		// accept()		
		printf("Waiting for connecting...\n");
		peer = accept(sock, NULL, NULL);
		if (peer < 0)
		{
			perror("accept()");
			close(sock);
			return -1;
		}
		printf("Connected!!\n");

		// recv()
		memset(buf, 0, sizeof(buf));
		printf("Waiting for receiving...\n");
		ret = stream_recv(peer, buf, sizeof(buf), 0);
		if (ret == -1)
		{
			perror("read()");
			close(peer);
			close(sock);
			return -1;
		}
		printf("Client said [%s]\n", buf);
	}
	
	// close()
	close(peer);
	close(sock);

	return 0;
}

// socket() -> connect() -> send() -> close()
static int do_client(void)
{
	int sock;
	struct sockaddr_un addr; // UNIX Domain Socket
	char buf[128];
	int ret;

	// sock()	
	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("socket()");
		return -1;
	}

	// Setting socket address(UNIX)
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path)-1);

	// connect()
	if (connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
	{
		perror("connect()");
		close(sock);
		return -1;
	}

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "this is msg from sock_stream");

	sleep(3);

	// send()
	ret = stream_send(sock, buf, sizeof(buf), 0);	
	if (ret < 0)
	{
		perror("send()");
		close(sock);
		return -1;
	}

	// close()
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
