#include <builtins/builtins.h>
#include <err.h>
#include <limits.h>
#include <stdio.h>

/**
 * @brief implementation of cd builtin command with '-' option only
 */
int builtin_cd(int argc, char **argv)
{
    // handle zero argument or the ~ argument
    if ((argc == 1) || (!strncmp(*(argv + 1), "~", 1)))
    {
        char *home_dir = getenv("HOME");
        if (home_dir == NULL)
        {
            warnx("cd: HOME not set");
            return 2;
        }
        char curr_dir[PATH_MAX];
        if (getcwd(curr_dir, PATH_MAX) == NULL)
            return 2;
        if (chdir(home_dir) != 0)
            return 2;
        setenv("OLDPWD", curr_dir, 1);
        return 0;
    }
    // check option '-'
    if (!strncmp(*(argv + 1), "-", 1))
    {
        char *oldpwd = getenv("OLDPWD");
        if (oldpwd == NULL)
        {
            warnx("cd: OLDPWD not set");
            return 2;
        }
        char curr_dir[PATH_MAX];
        if (getcwd(curr_dir, PATH_MAX) == NULL)
            return 2;
        if (chdir(oldpwd) != 0)
            return 2;
        setenv("OLDPWD", curr_dir, 1);
        printf("%s\n", oldpwd);
        return 0;
    }
    char curr_dir[PATH_MAX];
    if (getcwd(curr_dir, PATH_MAX) == NULL)
        return 2;
    setenv("OLDPWD", curr_dir, 1);
    int rc = chdir(*(argv + 1));
    setenv("PWD", *(argv + 1), 1);
    if (rc != 0)
    {
        warnx("cd: can't cd to %s", *(argv + 1));
        return 2;
    }
    return 0;
}