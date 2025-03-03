#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	pid_t pid;
	int a = 0;

	pid = fork();
	if (pid == 0)
	{
		// child process
		a = 3;
		printf("child-> a: %d\n", a);
	}
	else if (pid > 0)
	{
		// parent process
		a = 5;
		printf("child-> a: %d\n", a);
	}
	
	return 0;
}
