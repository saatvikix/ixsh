#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"

void execute_command(Command *command);
void execute_pipeline(Command *left, Command *right)
#endif
