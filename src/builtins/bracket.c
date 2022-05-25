#include <builtins/builtins.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>

enum bracket_op
{
    BRACKET_EQ,
    BRACKET_NE,
    BRACKET_GT,
    BRACKET_LT,
    BRACKET_GE,
    BRACKET_LE,
    BRACKET_UNKNOWN,
};

static bool is_int(char *str, int *res)
{
    bool success = true;
    size_t i = 0;
    while (str[i] != '\0' && success)
    {
        if (str[i] < '0' || str[i] > '9')
            success = false;
        i += 1;
    }
    if (success == false)
        return false;
    *res = atoi(str);
    return true;
}

static bool eval(int a, int b, enum bracket_op op)
{
    bool res = false;
    switch (op)
    {
    case BRACKET_EQ:
        res = a == b;
        break;
    case BRACKET_NE:
        res = a != b;
        break;
    case BRACKET_GT:
        res = a > b;
        break;
    case BRACKET_LT:
        res = a < b;
        break;
    case BRACKET_GE:
        res = a >= b;
        break;
    case BRACKET_LE:
        res = a <= b;
        break;
    default:
        res = false;
    }
    return res;
}

static enum bracket_op get_op(char *op)
{
    if (strcmp("-eq", op) == 0)
        return BRACKET_EQ;
    else if (strcmp("-ne", op) == 0)
        return BRACKET_NE;
    else if (strcmp("-gt", op) == 0)
        return BRACKET_GT;
    else if (strcmp("-lt", op) == 0)
        return BRACKET_LT;
    else if (strcmp("-ge", op) == 0)
        return BRACKET_GT;
    else if (strcmp("-le", op) == 0)
        return BRACKET_LT;
    else
        return BRACKET_UNKNOWN;
}

int builtin_bracket(int argc, char **argv,
                    __attribute__((unused)) struct runtime *rt)
{
    if (argc > 5)
    {
        warnx("[: too many arguments");
        return 1;
    }
    if (strcmp("]", argv[argc - 1]) != 0)
    {
        warnx("[: ']' expected");
        return 1;
    }
    if (argc == 3)
    {
        return 0;
    }
    if (argc == 2)
    {
        return 1;
    }

    enum bracket_op op = get_op(argv[2]);
    if (op == BRACKET_UNKNOWN)
    {
        warnx("[: %s: unexpected operator", argv[1]);
        return 1;
    }
    if (argc == 4)
    {
        warnx("[: %s: argument expected", argv[2]);
        return 1;
    }

    int a;
    if (!is_int(argv[1], &a))
    {
        warnx("[: Illegal number: %s", argv[1]);
        return 1;
    }
    int b;
    if (!is_int(argv[3], &b))
    {
        warnx("[: Illegal number: %s", argv[3]);
        return 1;
    }

    bool res = eval(a, b, op);

    return res ? 0 : 1;
}
