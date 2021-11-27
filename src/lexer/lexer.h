#pragma once

#include "token.h"

/**
 * @brief Structure that associates a string with a reserve_word type
 * example: 'if' -> RW_IF
 * (Simple aggregation)
 */
struct matching_rw
{
    const char *str;
    size_t len;
    enum rw_type type;
};

/**
 * @brief Structure that associates a string with a operator type
 * example: ';' -> OP_SEMICOLON
 *          '\n' -> OP_LINEFEED
 * (Simple aggregation)
 */
struct matching_op
{
    const char *str;
    size_t len;
    enum op_type type;
};

/**
 * @brief Main structure for the lexer
 * (complete agregation)
 */
struct lexer
{
    struct pretoken_vector *pretokens; ///< The vector of the pretokens
    size_t pretoken_index; ///< The current index of the pretoken vector
    size_t line_index; ///< The current index of the current command
    struct token *current_tok; ///< The next token, if processed
};

/**
 * \brief Creates a new lexer given an pretoken vector.
 */
struct lexer *lexer_new(struct pretoken_vector *pretokens);

/**
 ** \brief Free the given lexer with the pretoken vector.
 */
void lexer_free(struct lexer *lexer);

/**
 * \brief Returns the next token, but doesn't move forward: calling lexer_peek
 * multiple times in a row always returns the same result. This functions is
 * meant to help the parser check if the next token matches some rule.
 */
struct token *lexer_peek(struct lexer *lexer);

/**
 * \brief Returns the next token, and removes it from the stream:
 *   calling lexer_pop in a loop will iterate over all tokens until EOF.
 */
struct token *lexer_pop(struct lexer *lexer);

/*
 * @brief TODO
 */
struct token *get_next_token(struct lexer *lexer);
