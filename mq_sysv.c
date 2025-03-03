#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define IPC_KEY_FILENAME	"/proc"
#define IPC_KEY_PROJ_ID		'a'

struct msgbuf
{
	long mtype;
#define MSGBUF_STR_SIZE		64
	char string[MSGBUF_STR_SIZE];	
};

static void print_usage(const char *progname)
{
	printf("%s (send|recv) MTYPE\n", progname);
}

static int init_msgq(void)
{
	int msgq;
	key_t key;

	key = ftok(IPC_KEY_FILENAME, IPC_KEY_PROJ_ID);

	msgq = msgget(key, 0644 | IPC_CREAT);

	return msgq;
}

static int do_send(long mtype)
{
	int msgq;
	struct msgbuf mbuf;

	msgq = init_msgq();

	// Setting msgbuf
	memset(&mbuf, 0, sizeof(mbuf));
	mbuf.mtype = mtype;
	snprintf(mbuf.string, sizeof(mbuf.string), "Hello World mtype %ld", mtype);

	// send
	msgsnd(msgq, &mbuf, sizeof(mbuf.string), 0);

	return 0;
}

static int do_recv(long mtype)
{
	int msgq;
	struct msgbuf mbuf;

	msgq = init_msgq();

	memset(&mbuf, 0, sizeof(mbuf));
	
	// receive
	msgrcv(msgq, &mbuf, sizeof(mbuf.string), mtype, 0);
	printf("Received msg-> mtype: [%ld] / msg: [%s]\n", mbuf.mtype, mbuf.string);

	return 0;
}

// How to use: "./mq_sysv (send|recv) MTYPE"
int main(int argc, char **argv)
{
	int ret;
	long mtype;
		
	if (argc < 3)
	{
		print_usage(argv[0]);
		return -1;
	}

	mtype = strtol(argv[2], NULL, 10);

	if (!strcmp(argv[1], "send"))
	{
		if (mtype <= 0)
		{
			print_usage(argv[0]);
			return -1;
		}
		ret = do_send(mtype);
	}
	else if (!strcmp(argv[1], "recv"))
	{
		ret = do_recv(mtype);
	}
	else
	{
		print_usage(argv[0]);
		return -1;
	}	

	return ret;
}
