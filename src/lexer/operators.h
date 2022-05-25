#pragma once

enum op_type
{
    OP_UNKNOWN,
    OP_SEMICOLON,
    OP_LINEFEED,
    OP_REDIR,
    OP_PIPE,
    OP_OR,
    OP_AND,
    OP_LBRACE,
    OP_RBRACE,
    OP_LPARENTHESE,
    OP_RPARENTHESE,
    OP_DOUBLE_SEMICOLON
};

enum redir_type
{
    REDIR_GREAT, // '>'
    REDIR_LESS, // '<'
    REDIR_GREATAND, // '>&'
    REDIR_LESSAND, // '<&'
    REDIR_DGREAT, // '>>'
    REDIR_LESSGREAT, // '<>'
    REDIR_CLOBBER, // '>|'
};

struct redir_data
{
    enum redir_type type;
    int default_fd;
};

struct op_data
{
    enum op_type type;
    union
    {
        struct redir_data redir_data;
        void *null;
    } data;
};
