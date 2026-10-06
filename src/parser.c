#include "parser.h"
#include <stdlib.h>
#include <string.h>

Command parse_command(char *buffer)
{
    Command command;
    int capacity = 10;

    command.argc = 0;
    command.input_file = NULL;
    command.output_file = NULL;
    command.append = 0;

    command.argv = malloc(capacity * sizeof(char *));

    if (command.argv == NULL)
    {
        return command;
    }

    char *token = strtok(buffer, " \t");

    while (token != NULL)
    {

        // Input redirection
        if (strcmp(token, "<") == 0)
        {

            token = strtok(NULL, " \t");

            if (token != NULL)
            {
                command.input_file = token;
                command.append = 0;
            }
        }

        // Output redirection
        else if (strcmp(token, ">") == 0)
        {

            token = strtok(NULL, " \t");

            if (token != NULL)
            {
                command.output_file = token;
                command.append = 0;
            }
        }

        // Output redirection in append mode
        else if (strcmp(token, ">>") == 0)
        {

            token = strtok(NULL, " \t");

            if (token != NULL)
            {
                command.output_file = token;
                command.append = 1;
            }
        }

        else
        {

            if (command.argc >= capacity - 1)
            {
                capacity *= 2;

                char **temp = realloc(command.argv, capacity * sizeof(char *));

                if (temp == NULL)
                {
                    free(command.argv);
                    command.argc = 0;
                    command.argv = NULL;
                    return command;
                }

                command.argv = temp;
            }

            command.argv[command.argc] = token;
            command.argc++;
        }

        token = strtok(NULL, " \t");
    }

    command.argv[command.argc] = NULL;

    return command;
}

Pipeline parse_pipeline(char *buffer) 
{
    Pipeline pipeline;

    pipeline.commands = NULL;
    pipeline.count = 0;

    int command_count = 1;

    for (int i = 0; buffer[i] != '\0'; i++)
    {
        if (buffer[i] == '|')
        {
            command_count++;
        }
    }

    pipeline.commands = malloc(command_count * sizeof(Command));

    if (pipeline.commands == NULL) 
    {
        return pipeline;
    }

    char *segment_start = buffer;
    int command_index = 0;

    for (char *current = buffer; ; current++)
    {
        if (*current == '|' || *current == '\0')
        {
            int reached_end = (*current == '\0');
            
            *current = '\0';

            pipeline.commands[command_index] = parse_command(segment_start);
            command_index++;

            if (reached_end) break;

            segment_start = current + 1;
        }
    }

    pipeline.count = command_index;
    return pipeline;
}

void free_pipeline(Pipeline *pipeline)
{
    if (pipeline == NULL || pipeline->commands == NULL)
    {
        return;
    }

    for (int i = 0; i < pipeline->count; i++)
    {
        free(pipeline->commands[i].argv);
    }

    free(pipeline->commands);

    pipeline->commands = NULL;
    pipeline->count = 0;
}
