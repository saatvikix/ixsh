#include "executor.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>

void execute_command(Command *command) {
	
	pid_t pid = fork();

	if (pid < 0) {
		perror("fork");
		return;
	}
	
	// child process
	if (pid == 0) {
		execvp(command->argv[0], command->argv);

		// execvp exits only if it fails
		perror("execvp");
		exit(EXIT_FAILURE);
	}
	
	// parent process waits till the child gets completed
	wait(NULL);
}
