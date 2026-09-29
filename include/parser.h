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

Command parse_command(char *buffer);

#endif