#include "mypipe.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    char *argv_left[3] = { "echo", "Hallo", NULL };
    char *argv_right[4] = { "tr", "a", "e", NULL };
    // echo Hallo | tr a e
    // Hello
    return exec_pipe(argv_left, argv_right);
}

/**
 * @brief test
 *
 * @param argv_left function that will be piped
 * @param argv_right function in which left param will be piped
 * @return integer (0 if successful, -1 otherwise)
 */
int exec_pipe(char **argv_left, char **argv_right)
{
    // pipe
    int pipefds[2];
    pipe(pipefds);
    // fork
    pid_t pid = fork();
    if (pid == -1)
        return 1;
    if (pid == 0) // child
    {
        if (dup2(pipefds[1], STDOUT_FILENO) == -1) //  stdout to our queue
            errx(127, "child's dup2 failed");
        close(pipefds[0]);
        close(pipefds[1]);
        int e = execvp(argv_left[0], argv_left);
        if (e == -1)
            errx(127, "%s: command not found", argv_left[0]);
    }
    else // father
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);
        if (WEXITSTATUS(wstatus) == 127)
            errx(1, "%s: command not found", argv_left[0]);

        if (dup2(pipefds[0], STDIN_FILENO) == -1) //  our queue to stdin
            errx(127, "father's dup2 failed");
        close(pipefds[0]); // useful
        close(pipefds[1]);
        int e = execvp(argv_right[0], argv_right);
        if (e == -1)
            errx(127, "%s: command not found", argv_left[0]);
    }

    return 0;
}