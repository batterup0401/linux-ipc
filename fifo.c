#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO_FILENAME "./testfifo"

static void print_usage(char *progname)
{
	printf("%s (w|r)\n", progname);
	
	return;
}

static int do_reader(void)
{
	int fd;
	char buf[128];

	printf("Call open() API...\n");
	fd = open(FIFO_FILENAME, O_RDONLY); // blocking

	read(fd, buf, sizeof(buf));
	printf("writer said [%s]\n", buf);

	close(fd);
	
	return 0;
}

static int do_writer(void)
{
	int fd;
	char buf[128];

	unlink(FIFO_FILENAME);
	
	printf("Make fifo!\n");
	mkfifo(FIFO_FILENAME, 0644);
	
	printf("Call open() API...\n");
	fd = open(FIFO_FILENAME, O_WRONLY); // blocking

	strncpy(buf, "Hello", sizeof(buf));
	
	write(fd, buf, strlen(buf));

	close(fd);

	return 0;
}

// How to use: "./fifo (w|r)"
int main(int argc, char **argv)
{
	if (argc < 2)
	{
		print_usage(argv[0]);	
	}

	if(!strcmp(argv[1], "r"))
	{
		do_reader();
	}
	else if(!strcmp(argv[1], "w"))
	{
		do_writer();
	}
	else
	{
		print_usage(argv[0]);	
		return -1;
	}

	return 0;
}
