#include <err.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc < 3)
        return 2;
    // opening the file
    int fd = open(argv[1], O_WRONLY | O_CREAT, 0644);
    if (fd < 0)
        return 1;
    // fork
    pid_t pid = fork();
    if (pid == -1)
        return 1;
    if (pid == 0) // child
    {
        dup2(fd, STDOUT_FILENO);
        int e = execvp(argv[2], argv + 2);
        if (e == -1)
        {
            return 127;
        }
    }
    else // father
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);
        if (WEXITSTATUS(wstatus) == 127)
        {
            close(fd);
            errx(1, "%s: command not found", argv[0]);
        }
        printf("%s exited with %d!\n", argv[2], WEXITSTATUS(wstatus));
    }
    return 0;
}