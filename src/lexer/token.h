#pragma once

#include <stdbool.h>
#include <unistd.h>

enum token_type
{
    TOKEN_WORD,
    TOKEN_RW,
    TOKEN_OP,
    TOKEN_EOF
};

enum op_type
{
    OP_UNKNOWN,
    OP_SEMICOLON,
    OP_LINEFEED,
    OP_PIPE
};

enum rw_type
{
    RW_UNKNOWN,
    RW_IF,
    RW_FI,
    RW_THEN,
    RW_ELIF,
    RW_ELSE
};

struct token
{
    enum token_type type;
    union
    {
        enum rw_type rw_type;
        enum op_type op_type;
        char *word;
    } data;
};

/**
 * \brief Allocate a new token word
 */
struct token *token_new_word(char *word, size_t len);
/**
 * \brief Allocate a new token reserve word
 */
struct token *token_new_rw(enum rw_type type);
/**
 * \brief Allocate a new token operator
 */
struct token *token_new_op(enum op_type type);
/**
 * \brief Allocate a new token EOF
 */
struct token *token_new_eof(void);

/**
 * \brief Frees a token
 */
void token_free(struct token *token);

/**
 * \brief Check if the token is an operator and is = op_type
 */
bool is_op(struct token *tok, enum op_type op_type);

/**
 * \brief Check if the token is a reserved word and is = rw_type
 */
bool is_rw(struct token *tok, enum rw_type rw_type);

/**
 * \brief Check if the token is a word
 */
bool is_word(struct token *tok);
