#include "redirection.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int apply_redirection(Command *command)
{
    // Redirect standard input from a file
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

    // Redirect standard output to a file
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

        if (fd < 0) {
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
