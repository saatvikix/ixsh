#ifndef PARSER_H
#define PARSER_H

typedef struct
{
    int argc;
    char **argv;

    char *input_file;
    char *output_file;

    int append;
} Command;

typedef struct 
{
    Command *commands;
    int count;
} Pipeline;

Command parse_command(char *buffer);
Pipeline parse_pipeline(char *buffer);
void free_pipeline(Pipeline *pipeline);

#endif