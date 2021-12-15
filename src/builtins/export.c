#include <builtins/builtins.h>
#include <ctype.h>
#include <err.h>
#include <stdio.h>

int builtin_export(int argc, char **argv, struct runtime *rt)
{
    rt->encountered_exit = true;
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

    return return_status;

illegal_argument:
    errx(2, "exit: Illegal number: %s", argv[1]);
    // warnx("exit: Illegal number: %s", argv[1]);
    // return 2;
}
