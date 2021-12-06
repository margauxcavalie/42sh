#pragma once

#include <parser/simple_cmd/ast_simple_cmd.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <utils/alloc.h>
#include <vector/vector.h>

struct matching_builtin
{
    char *cmd;
    int (*exec)(int argc, char **argv, struct runtime *rt);
};

int builtin_echo(int argc, char **argv,
                 __attribute__((unused)) struct runtime *rt);
int builtin_continue(int argc, char **argv, struct runtime *rt);
int exec_builtin(struct ast_simple_cmd *ast, bool *is_builtin,
                 struct runtime *rt);
