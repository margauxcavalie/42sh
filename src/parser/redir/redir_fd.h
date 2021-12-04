#pragma once

#include <parser/redir/ast_redir.h>

int redir_in_out(struct ast_redir *ast_redir, int *saved_fd, bool *is_file,
                 bool noclobber);
