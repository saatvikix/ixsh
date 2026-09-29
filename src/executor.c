#include "executor.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>

int apply_redirection(Command *command)
{

	// input redirection
	if (command->input_file != NULL)
	{
		int fd = open(command->input_file, O_RDONLY);

		if (fd < 0)
		{
			perror("open");
			return -1;
		}

		if (dup2(fd, STDIN_FILENO) < 0)
		{
			perror("dup2");
			close(fd);
			return -1;
		}

		close(fd);
	}

	// output redirection
	if (command->output_file != NULL)
	{
		int flags = O_WRONLY | O_CREAT;

		if (command->append)
		{
			flags |= O_APPEND;
		}
		else
		{
			flags |= O_TRUNC;
		}

		int fd = open(command->output_file, flags, 0644);

		if (fd < 0)
		{
			perror("open");
			return -1;
		}

		if (dup2(fd, STDOUT_FILENO) < 0)
		{
			perror("dup2");
			close(fd);
			return -1;
		}

		close(fd);
	}

	return 0;
}

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
