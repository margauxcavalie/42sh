#include <builtins/builtins.h>
#include <ctype.h>
#include <err.h>
#include <stdio.h>

int builtin_exit(int argc, char **argv,
                 __attribute__((unused)) struct runtime *rt)
{
    int return_status;
    if (argc == 1)
        return_status = 0;

    else if (argc >= 2)
    {
        // special case where exit is "exit 0"
        if (!(strcmp(argv[1], "0")))
            return_status = 0;
        else
        {
            return_status = atoi(argv[1]);
            if (return_status <= 0 || return_status > 255)
                goto illegal_argument;
        }
    }

    rt->encountered_exit = true;
    return return_status;

illegal_argument:
    errx(2, "exit: Illegal number: %s", argv[1]);
}
