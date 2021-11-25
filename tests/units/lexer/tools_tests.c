#include <criterion/criterion.h>

#include <stdio.h>
#include <string.h>

#include <lexer/lexer.h>

Test(get_next_token, empty_token)
{
    char *str = "";
    size_t size = 0;
    struct token *tok = get_next_token(str, &size);
    cr_assert_eq(tok->type, TOKEN_EOF);
    cr_assert_eq(size, 0);
    token_free(tok);
}

Test(get_next_token, space)
{
    char *str = "  \t\r  ";
    size_t size = 0;
    struct token *tok = get_next_token(str, &size);
    cr_assert_eq(tok->type, TOKEN_EOF);
    cr_assert_eq(size, 6);
    token_free(tok);
}

Test(get_next_token, if_basic)
{
    char *str = "if";
    size_t size = 0;
    struct token *tok = get_next_token(str, &size);
    cr_assert_eq(tok->type, TOKEN_IF);
    cr_assert_eq(size, 2);
    token_free(tok);
}


Test(get_next_token, if_spaces)
{
    char *str = "   if   ";
    size_t size = 0;
    struct token *tok = get_next_token(str, &size);
    cr_assert_eq(tok->type, TOKEN_IF);
    cr_assert_eq(size, 5);
    token_free(tok);
}

Test(get_next_token, if_with_another_token)
{
    char *str = "if   else";
    size_t size = 0;
    struct token *tok = get_next_token(str, &size);
    cr_assert_eq(tok->type, TOKEN_IF);
    cr_assert_eq(size, 2);
    token_free(tok);
}

Test(get_next_token, word1)
{
    char *str = "salout";
    size_t size = 0;
    struct token *tok = get_next_token(str, &size);
    cr_assert_eq(tok->type, TOKEN_WORD);
    cr_assert_str_eq(tok->word, "salout");
    cr_assert_eq(size, 6);
    token_free(tok);
}

Test(get_next_token, word2)
{
    char *str = "  _";
    size_t size = 0;
    struct token *tok = get_next_token(str, &size);
    cr_assert_eq(tok->type, TOKEN_WORD);
    cr_assert_str_eq(tok->word, "_");
    cr_assert_eq(size, 3);
    token_free(tok);
}

Test(get_next_token, word3)
{
    char *str = "ifif";
    size_t size = 0;
    struct token *tok = get_next_token(str, &size);
    cr_assert_eq(tok->type, TOKEN_WORD);
    cr_assert_str_eq(tok->word, "ifif");
    cr_assert_eq(size, 4);
    token_free(tok);
}

Test(get_next_token, then_basic)
{
    char *str = "then";
    size_t size = 0;
    struct token *tok = get_next_token(str, &size);
    cr_assert_eq(tok->type, TOKEN_THEN);
    cr_assert_eq(size, 4);
    token_free(tok);
}

Test(get_next_token, elif_basic)
{
    char *str = "elif";
    size_t size = 0;
    struct token *tok = get_next_token(str, &size);
    cr_assert_eq(tok->type, TOKEN_ELIF);
    cr_assert_eq(size, 4);
    token_free(tok);
}

Test(get_next_token, else_basic)
{
    char *str = "else";
    size_t size = 0;
    struct token *tok = get_next_token(str, &size);
    cr_assert_eq(tok->type, TOKEN_ELSE);
    cr_assert_eq(size, 4);
    token_free(tok);
}

Test(get_next_token, fi_basic)
{
    char *str = "fi";
    size_t size = 0;
    struct token *tok = get_next_token(str, &size);
    cr_assert_eq(tok->type, TOKEN_FI);
    cr_assert_eq(size, 2);
    token_free(tok);
}

Test(get_next_token, semicolon_basic)
{
    char *str = ";";
    size_t size = 0;
    struct token *tok = get_next_token(str, &size);
    cr_assert_eq(tok->type, TOKEN_SEMICOLON);
    cr_assert_eq(size, 1);
    token_free(tok);
}

Test(get_next_token, linefeed_basic)
{
    char *str = "\n";
    size_t size = 0;
    struct token *tok = get_next_token(str, &size);
    cr_assert_eq(tok->type, TOKEN_LINEFEED);
    cr_assert_eq(size, 1);
    token_free(tok);
}

Test(get_next_token, single_quote_basic)
{
    char *str = "\'";
    size_t size = 0;
    struct token *tok = get_next_token(str, &size);
    cr_assert_eq(tok->type, TOKEN_SINGLE_QUOTE);
    cr_assert_eq(size, 1);
    token_free(tok);
}

