#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define POSIX_MQ_NAME		"/testlip"
#define MSGBUF_STR_SIZE 	8192

static void print_usage(const char *progname)
{
	printf("%s (send|recv) [PRIORITY]\n", progname);
}

static int init_msgq(void)
{
	mqd_t mqd;
	struct mq_attr attr;

	mqd = mq_open(POSIX_MQ_NAME, O_RDWR | O_CREAT, 0644, NULL);

	memset(&attr, 0, sizeof(attr));
	mq_getattr(mqd, &attr);	

	// Check message queue attribute
#if 0
	printf("mq_flags: %ld\n", attr.mq_flags);		/* Flags: 0 or O_NONBLOCK */
        printf("mq_maxmsg: %ld\n", attr.mq_maxmsg);		/* Max. # of messages on queue */
        printf("mq_msgsize: %ld\n", attr.mq_msgsize);		/* Max. message size (bytes) */
        printf("mq_curmsgs: %ld\n", attr.mq_curmsgs);		/* # of messages currently in queue */
#endif
	return mqd;
}

static int do_send(unsigned int priority)
{
	int mqd;
	char buf[MSGBUF_STR_SIZE];

	mqd = init_msgq();

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "Hello World from process[pid: %d]", getpid());

	mq_send(mqd, buf, sizeof(buf), priority);
	printf("sent msg: %s\n", buf);

	mq_close(mqd);

	return 0;
}

static int do_recv(void)
{
	int mqd;
	char buf[MSGBUF_STR_SIZE];
	unsigned int priority;

	mqd = init_msgq();

	memset(buf, 0, sizeof(buf));
	mq_receive(mqd, buf, sizeof(buf), &priority);
	printf("priority: %d / msg: %s\n", priority, buf);

	mq_close(mqd);

	return 0;	
}

int main(int argc, char **argv)
{
	int ret;

	if (argc < 2)
	{
		print_usage(argv[0]);
	}
	
	if (!strcmp(argv[1], "send"))
	{
		if (argc < 3)
		{
			print_usage(argv[0]);
			return -1;
		}

		unsigned int priority = strtoul(argv[2], NULL, 10);
		
		ret = do_send(priority);
	}
	else if (!strcmp(argv[1], "recv"))
	{
		ret = do_recv();
	}
	else 
	{
		print_usage(argv[0]);
		return -1;
	}

	return ret;
}
