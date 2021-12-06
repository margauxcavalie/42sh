#include <builtins/builtins.h>
#include <ctype.h>
#include <err.h>
#include <stdio.h>

int builtin_continue(int argc, char **argv, struct runtime *rt)
{
    if (argc == 1)
        rt->loops_to_break = 0;

    else if (argc >= 2)
    {
        int number = atoi(argv[1]);
        if (number <= 0)
            goto illegal_argument;

        else
            rt->loops_to_break = number - 1;
    }

    rt->encountered_continue = true;
    if (rt->loops_to_break > rt->loops_count)
        rt->loops_to_break = rt->loops_count;

    return 0;

illegal_argument:
    errx(2, "continue: Illegal number: %s", argv[1]);
}
