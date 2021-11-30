#include <builtins/builtins.h>
#include <stdio.h>

/*
void replace_opt_e(int argc, char **argv)
{
    char **new_argv = zalloc(sizeof(char *) * argc);
    for (int i = 0; i < argc; i++)
    {
        int len = strlen(argv[i]);
        new_argv[i] = zalloc(sizeof(char) * (len + 1));
        for (int j = 0; j < len - 1; j++)
        {
            if ((argv[i][j] == '\\') && (argv[i][j + 1] == 'n'))
            {
                new_argv[i][j] = '\n';
                j += 1;
            }
            else if ((argv[i][j] == '\\') && (argv[i][j + 1] =='t'))
            {
                new_argv[i][j] = '\t';
                j += 1;
            }
            else if ((argv[i][j] == '\\') && (argv[i][j + 1] == '\\'))
            {
                new_argv[i][j] = '\\';
                j += 1;
            }
            else
            {
                new_argv[i][j] = argv[i][j];

            }
        }
        new_argv[i][len - 1] = argv[i][len - 1];
    }
    argv = new_argv;
}*/

int builtin_echo(int argc, char **argv)
{
    bool opt_state = true;

    bool opt_n = false;
    bool opt_e = false;
    opt_e = opt_e;
    int i = 1;
    while (opt_state && i < argc)
    {
        if (!strcmp("-n", argv[i]))
        {
            opt_n = true;
        }
        else if (!strcmp("-e", argv[i]))
        {
            opt_e = true;
        }
        else if (!strcmp("-ne", argv[i]) || !strcmp("-en", argv[i]))
        {
            opt_n = true;
            opt_e = true;
        }
        else
        {
            opt_state = false;
            break;
        }
        i += 1;
    }
    /*if (opt_e == true)
    {
        replace_opt_e(argc, argv);
    }*/
    for (int i = 1; i < argc - 1; i++)
    {
        printf("%s ", argv[i]);
    }
    if (argc > 1)
    {
        printf("%s", argv[argc - 1]);
    }
    if (opt_n == false)
    {
        printf("\n");
    }
    /*if (opt_e == true)
    {
        for (int i = 0; i < argc; i++)
        {
            free(argv[i]);
        }
        free(argv);
    }*/
    return 0;
}
