#include <builtins/builtins.h>
#include <err.h>
#include <stdio.h>
#include <var_expansion/var_expansion.h>

/**
 * @brief implementation of cd builtin command with '-' option only
 */
int builtin_cd(int argc, char **argv,
               __attribute__((unused)) struct runtime *rt)
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
        char curr_dir[4096];
        if (getcwd(curr_dir, 4096) == NULL)
            return 2;
        if (chdir(home_dir) != 0)
            return 2;
        setenv("OLDPWD", curr_dir, 1);
        char *curr_dir_to_send = strdup(curr_dir);
        char *oldpwd_to_send = zalloc(sizeof(char) * 7);
        strcpy(oldpwd_to_send, "OLDPWD");
        var_hash_map_insert(rt->variables, oldpwd_to_send, curr_dir_to_send);
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
        char curr_dir[4096];
        if (getcwd(curr_dir, 4096) == NULL)
            return 2;
        if (chdir(oldpwd) != 0)
            return 2;
        setenv("OLDPWD", curr_dir, 1);
        char *curr_dir_to_send = strdup(curr_dir);
        char *oldpwd_to_send = zalloc(sizeof(char) * 7);
        strcpy(oldpwd_to_send, "OLDPWD");
        var_hash_map_insert(rt->variables, oldpwd_to_send, curr_dir_to_send);
        printf("%s\n", oldpwd);
        return 0;
    }
    char curr_dir[4096];
    if (getcwd(curr_dir, 4096) == NULL)
        return 2;
    setenv("OLDPWD", curr_dir, 1);
    char *curr_dir_to_send = strdup(curr_dir);
    char *oldpwd_to_send = zalloc(sizeof(char) * 7);
    strcpy(oldpwd_to_send, "OLDPWD");
    var_hash_map_insert(rt->variables, oldpwd_to_send, curr_dir_to_send);

    int rc = chdir(*(argv + 1));
    setenv("PWD", *(argv + 1), 1);
    if (getcwd(curr_dir, 4096) == NULL)
        return 2;
    char *curr_dir_to_send_pwd = strdup(curr_dir);
    char *pwd_to_send = zalloc(sizeof(char) * 4);
    strcpy(pwd_to_send, "PWD");
    var_hash_map_insert(rt->variables, pwd_to_send, curr_dir_to_send_pwd);
    // hash_map_insert(rt->variables, "PWD", *(argv + 1), free);
    if (rc != 0)
    {
        warnx("cd: can't cd to %s", *(argv + 1));
        return 2;
    }
    return 0;
}
