#include "options.h"

#include <err.h>
#include <getopt.h>
#include <stdio.h>

static struct option long_options[] = { { "pretty-print", 0, NULL, 'p' },
                                        { NULL, 0, NULL, 0 } };

static struct opts get_default_opts()
{
    struct opts opts = { .pretty_print = false,
                         .type = INPUT_PROMPT,
                         .cmd = NULL,
                         .path = NULL,
                         .ind_script_args = 0 };
    return opts;
}

static void args_error(const char *str)
{
    errx(2, "Arguments syntax error : %s", str);
}

struct opts get_options(int argc, char **argv)
{
    struct opts opts = get_default_opts();
    char ch;
    while ((ch = getopt_long(argc, argv, "pc:", long_options, NULL)) != -1)
    {
        // check to see if a single character or long option came through
        switch (ch)
        {
        case 'p':
            opts.pretty_print = true;
            break;
        case 'c':
            opts.cmd = optarg;
            opts.type = INPUT_STRING;
            break;
        }
    }

    if (optind < argc)
    {
        if (opts.type == INPUT_STRING)
            args_error("Cannot set scipts in string mode");
        opts.type = INPUT_FILE;
        int nb = argc - optind;
        if (nb > 1)
        {
            opts.ind_script_args = optind;
        }
        opts.path = argv[optind];
    }

    return opts;
}
