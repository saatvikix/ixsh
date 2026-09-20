#ifndef PARSER_H
#define PARSER_H

typedef struct {
    int argc;
    char **argv;
} Command;

Command parse_command(char *buffer);

#endif