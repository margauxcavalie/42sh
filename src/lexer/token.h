#pragma once

#include <stdbool.h>
#include <unistd.h>

#include "operators.h"
#include "rw.h"

enum token_type
{
    TOKEN_WORD,
    TOKEN_RW,
    TOKEN_FNAME,
    TOKEN_IONUMBER,
    TOKEN_OP,
    TOKEN_ASSIGNEMENT_WORD,
    TOKEN_EOF
};

struct token
{
    enum token_type type;
    union
    {
        enum rw_type rw_type;
        struct op_data op_data;
        int io_number;
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
 * \brief Allocate a new token io number
 */
struct token *token_new_ionumber(int number);
/**
 * \brief Allocate a new token operator
 */
struct token *token_new_op(struct op_data op_data);
/**
 * \brief Allocate a new token assignement_word
 */
struct token *token_new_assignement_word(char *word, size_t len);
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
/**
 * \brief Check if the token is a ionumber
 */
bool is_ionumber(struct token *tok);

/**
 * @brief Check if the token is a fname
 *
 */
bool is_fname(struct token *tok);

/**
 * \brief Check if the token is a assignement word
 */
bool is_assignement_word(struct token *tok);
