#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"

int apply_redirection(Command *command);
void execute_command(Command *command);

#endif
