#pragma once

#include <stdbool.h>

enum input_type
{
    INPUT_STRING,
    INPUT_PROMPT,
    INPUT_FILE
};

struct opts
{
    bool pretty_print;
    enum input_type type;
    char *cmd;
    char *path;
};

struct opts get_options(int argc, char **argv);
