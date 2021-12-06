#include "redir_fd.h"

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

static bool fd_is_valid(int fd)
{
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

static bool is_int(char *str, int *res)
{
    bool success = true;
    size_t i = 0;
    while (str[i] != '\0' && success)
    {
        if (str[i] < '0' || str[i] > '9')
            success = false;
        i += 1;
    }
    if (success == false)
        return false;
    *res = atoi(str);
    return true;
}

static bool is_regular_file(const char *path)
{
    struct stat sb;
    if (stat(path, &sb) == 0 && S_ISREG(sb.st_mode))
    {
        return true;
    }
    return false;
}

static int redir_fd(int file_fd, int fd)
{
    int saved_fd = dup(fd);
    dup2(file_fd, fd);
    return saved_fd;
}

static bool open_file_in_out(const char *path, int *file_fd, int flags)
{
    *file_fd = open(path, O_RDWR | O_CREAT | flags, 0644);
    if (*file_fd == -1)
    {
        if (errno == EISDIR)
            warnx("cannot open %s: Is a directory", path);
        else if (access(path, R_OK) == -1)
        {
            if (errno == EACCES)
                warnx("cannot open %s: Permission denied", path);
            else
                warnx("cannot open %s: Unknwon reason", path);
        }
        return false;
    }
    return true;
}

static bool open_file_in(const char *path, int *file_fd)
{
    *file_fd = open(path, O_RDONLY);
    if (*file_fd == -1)
    {
        if (errno == EISDIR)
            warnx("cannot open %s: Is a directory", path);
        else if (access(path, F_OK) == -1)
            warnx("cannot open %s: no such file or directory", path);
        else if (access(path, R_OK) == -1)
        {
            if (errno == EACCES)
                warnx("cannot open %s: Permission denied", path);
            else
                warnx("cannot open %s: Unknwon reason", path);
        }
        return false;
    }
    return true;
}

static bool open_file_out(const char *path, int *file_fd, int flags,
                          bool noclobber)
{
    if (is_regular_file(path) && noclobber)
    {
        warnx("cannot create %s: File exist", path);
        return false;
    }

    *file_fd = open(path, O_WRONLY | O_CREAT | flags, 0644);
    if (*file_fd == -1)
    {
        if (errno == EISDIR)
            warnx("cannot create %s: Is a directory", path);
        else if (access(path, W_OK) == -1)
        {
            if (errno == EACCES)
                warnx("cannot create %s: Permission denied", path);
            else
                warnx("cannot create %s: Unknwon reason", path);
        }
        return false;
    }
    return true;
}

int redir_in_out(struct ast_redir *ast_redir, char *path, int *saved_fd,
                 bool noclobber)
{
    int fd = ast_redir->fd;
    enum redir_type type = ast_redir->type;

    int file_fd;
    bool success = false;
    if (type == REDIR_GREAT)
        success = open_file_out(path, &file_fd, O_TRUNC, noclobber);
    else if (type == REDIR_CLOBBER) // same as REDIR_GREAT but with noclobber
        success = open_file_out(path, &file_fd, O_TRUNC, false);
    else if (type == REDIR_DGREAT)
        success = open_file_out(path, &file_fd, O_APPEND, false);
    else if (type == REDIR_LESS)
        success = open_file_in(path, &file_fd);
    else if (type == REDIR_LESSGREAT)
        success = open_file_in_out(path, &file_fd, 0);
    else if (type == REDIR_LESSAND || type == REDIR_GREATAND)
    {
        int fd_target;
        if (!is_int(path, &fd_target) || !fd_is_valid(fd_target))
        {
            warnx("%s: bad file descriptor", path);
            return -1;
        }
        file_fd = dup(fd_target);
        success = true;
    }
    else
    {
        warnx("unsupported redir type");
        return -1;
    }
    if (success == false)
        return -1;

    *saved_fd = redir_fd(file_fd, fd);
    return file_fd;
}
