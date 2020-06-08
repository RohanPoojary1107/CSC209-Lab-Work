#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{

	int i;
	int n;
	int num_kids;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: childcreates <numkids>\n");
		exit(1);
	}

	num_kids = strtol(argv[1], NULL, 10);
	int parent = getppid();
	for (i = 0; i < num_kids; i++)
	{
		int status;
		if (parent == getppid())
		{
			parent = getpid();
			n = fork();
		}
		else if (n < 0)
		{
			perror("fork");
			exit(1);
		}
		else
		{
			continue;
		}
		printf("pid = %d, ppid = %d, i = %d\n", getpid(), getppid(), i);
		wait(&status);
	}

	return 0;
}
