#pragma once

#include <parser/redir/ast_redir.h>

int redir_in_out(struct ast_redir *ast_redir, char *path, int *saved_fd,
                 bool noclobber);
