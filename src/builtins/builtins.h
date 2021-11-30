#pragma once

#include <parser/ast_simple_cmd_node.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <utils/alloc.h>
#include <vector/vector.h>

struct matching_builtin
{
    char *cmd;
    int (*exec)(int argc, char **argv);
};

int builtin_echo(int argc, char **argv);
void replace_opt_e(int argc, char **argv);
int exec_builtin(struct ast_simple_cmd_node *ast, bool *is_builtin);
