#include <builtins/builtins.h>
#include <stdio.h>


static char *replace_opt_e(int size, const char *src)
{
    char *dest = zalloc(sizeof(char) * (size + 1));
    int dest_i = 0;
    for (int i = 0; i < size; i++)
    {
        if ((src[i] == '\\') && (src[i + 1] == 'n'))
        {
            dest[dest_i] = '\n';
            i += 1;
            dest_i += 1;
        }
        else if ((src[i] == '\\') && (src[i + 1] == 't'))
        {
            dest[dest_i] = '\t';
            i += 1;
            dest_i += 1;
        }
        else if ((src[i] == '\\') && (src[i + 1] == '\\'))
        {
            dest[dest_i] = '\\';
            i += 1;
            dest_i += 1;
        }
        else
        {
            dest[dest_i] = src[i];
            dest_i += 1;
        }
    }
    return dest;
}

int builtin_echo(int argc, char **argv)
{
    bool opt_state = true;

    bool opt_n = false;
    bool opt_e = false;

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
    if (opt_e == true)
    {
        int j = i;
        while (j < argc)
        {
            char *new_str = replace_opt_e(strlen(argv[j]), argv[j]);
            argv[j] = new_str;
            j += 1;
        }        
    }
    
    for (int j = i; j < argc; j++)
    {
        printf("%s", argv[j]);
        if (j != argc - 1)
            printf(" ");
    }
    if (opt_n == false)
    {
        printf("\n");
    }
    if (opt_e == true)
    {
        for (int j = i; j < argc; j++)
        {
            free(argv[j]);
        }
    }
    return 0;
}
