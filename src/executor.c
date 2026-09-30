#include "executor.h"
#include "redirection.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>

void execute_command(Command *command)
{

	pid_t pid = fork();

	if (pid < 0)
	{
		perror("fork");
		return;
	}

	// child process
	if (pid == 0)
	{
		if (apply_redirection(command) < 0)
		{
			exit(EXIT_FAILURE);
		}

		execvp(command->argv[0], command->argv);

		perror("execvp");
		exit(EXIT_FAILURE);
	}

	// parent process waits till the child gets completed
	wait(NULL);
}
