#include <builtins/builtins.h>
#include <err.h>
#include <stdio.h>

int builtin_read(int argc, char **argv, struct runtime *rt)
{
    if (argc != 2)
    {
        warnx("read: syntax error");
        return 1;
    }

    size_t capacity = 8;
    size_t size = 0;
    char *str = zalloc(sizeof(char) * (capacity + 1));

    char c;
    ssize_t e;
    while ((e = read(STDIN_FILENO, &c, 1)) > 0)
    {
        if (c == '\n')
        {
            break;
        }
        if (size + 1 >= capacity)
        {
            str = xrealloc(str, sizeof(char) * ((size + 1) * 2 + 1));
            capacity = (size + 1) * 2;
        }
        str[size++] = c;
    }
    str[size] = '\0';

    char *name = strdup(argv[1]);
    var_hash_map_insert(rt->variables, name, str);

    if (e <= 0)
        return 1;
    return 0;
}
