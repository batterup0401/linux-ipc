#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	int pipe_fds[2];
	pid_t pid;
	int wstatus;

	char buf[1024];
	memset(buf, 0, sizeof(buf));

	printf("Starting in pid[%d]\n", getpid());

	// Create pipe
	pipe(pipe_fds);

	// Check pipe fds
	printf("Reader side fd: %d\n", pipe_fds[0]);
	printf("Writer side fd: %d\n", pipe_fds[1]);

	pid = fork();
	if (pid == 0)
	{
		// child process
		close(pipe_fds[1]);

		read(pipe_fds[0], buf, sizeof(buf));
		printf("Child process[%d]: my parent said [%s]\n", getpid(), buf);

		close(pipe_fds[0]);	
	}
	else if (pid > 0)
	{
		// parent process
		close(pipe_fds[0]);

		strncpy(buf, "Hello Child~", sizeof(buf)-1);

		write(pipe_fds[1], buf, strlen(buf));
		
		close(pipe_fds[1]);

		wait(&wstatus);
	}
	
	return 0;
}
