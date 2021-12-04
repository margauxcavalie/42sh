#include <criterion/criterion.h>
#include <lexer/lexer.h>
#include <prelexer/prelexer.h>

/*
 * Tests the main purpose of the lexer
 */

Test(lexer, empty)
{
    char *s = "";
    struct pretoken_vector *vec = prelexify(s);
    struct lexer *lexer = lexer_new(vec);
    cr_assert_eq(lexer->current_tok->type, TOKEN_EOF);
    lexer_free(lexer); // free vec inside
}

Test(lexer, spaces)
{
    char *s = "  \t  ";
    struct pretoken_vector *vec = prelexify(s);
    struct lexer *lexer = lexer_new(vec);
    cr_assert_eq(lexer->current_tok->type, TOKEN_EOF);
    lexer_free(lexer); // free vec inside
}
Test(lexer, simple_word)
{
    char *s = "jesuisunmot";
    struct pretoken_vector *vec = prelexify(s);
    struct lexer *lexer = lexer_new(vec);
    struct token *tok;

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_WORD);
    cr_assert_str_eq(tok->data.word, "jesuisunmot");
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_EOF);

    lexer_free(lexer); // free vec inside
}
Test(lexer, simple_word_big)
{
    char *s = "jesuisunquoshdfiuqsdhfiujqsdhfshdfjuqhdjkdhfjkqhfkjqsdhmot";
    struct pretoken_vector *vec = prelexify(s);
    struct lexer *lexer = lexer_new(vec);
    struct token *tok;

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_WORD);
    cr_assert_str_eq(
        tok->data.word,
        "jesuisunquoshdfiuqsdhfiujqsdhfshdfjuqhdjkdhfjkqhfkjqsdhmot");
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_EOF);

    lexer_free(lexer); // free vec inside
}
Test(lexer, simple_word_special_chars)
{
    char *s = "jqufk*-jq\r\rd\vhmot";
    struct pretoken_vector *vec = prelexify(s);
    struct lexer *lexer = lexer_new(vec);
    struct token *tok;

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_WORD);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_EOF);

    lexer_free(lexer); // free vec inside
}
Test(lexer, simple_rw_if)
{
    char *s = "if";
    struct pretoken_vector *vec = prelexify(s);
    struct lexer *lexer = lexer_new(vec);
    struct token *tok;

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_RW);
    cr_assert_eq(tok->data.rw_type, RW_IF);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_EOF);

    lexer_free(lexer); // free vec inside
}
Test(lexer, simple_op_semicolon)
{
    char *s = ";";
    struct pretoken_vector *vec = prelexify(s);
    struct lexer *lexer = lexer_new(vec);
    struct token *tok;

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_OP);
    cr_assert_eq(tok->data.op_data.type, OP_SEMICOLON);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_EOF);

    lexer_free(lexer); // free vec inside
}
Test(lexer, word_with_op)
{
    char *s = "salut\n";
    struct pretoken_vector *vec = prelexify(s);
    struct lexer *lexer = lexer_new(vec);
    struct token *tok;

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_WORD);
    cr_assert_str_eq(tok->data.word, "salut");
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_OP);
    cr_assert_eq(tok->data.op_data.type, OP_LINEFEED);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_EOF);

    lexer_free(lexer); // free vec inside
}

Test(lexer, hard1)
{
    char *s = "if then; then fi\n fi";
    struct pretoken_vector *vec = prelexify(s);
    struct lexer *lexer = lexer_new(vec);
    struct token *tok;

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_RW);
    cr_assert_eq(tok->data.rw_type, RW_IF);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_RW);
    cr_assert_eq(tok->data.rw_type, RW_THEN);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_OP);
    cr_assert_eq(tok->data.op_data.type, OP_SEMICOLON);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_RW);
    cr_assert_eq(tok->data.rw_type, RW_THEN);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_RW);
    cr_assert_eq(tok->data.rw_type, RW_FI);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_OP);
    cr_assert_eq(tok->data.op_data.type, OP_LINEFEED);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_RW);
    cr_assert_eq(tok->data.rw_type, RW_FI);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_EOF);

    lexer_free(lexer); // free vec inside
}

Test(lexer, hard2)
{
    char *s = "if ok1; then ok2\n fi";
    struct pretoken_vector *vec = prelexify(s);
    struct lexer *lexer = lexer_new(vec);
    struct token *tok;

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_RW);
    cr_assert_eq(tok->data.rw_type, RW_IF);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_WORD);
    cr_assert_str_eq(tok->data.word, "ok1");
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_OP);
    cr_assert_eq(tok->data.op_data.type, OP_SEMICOLON);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_RW);
    cr_assert_eq(tok->data.rw_type, RW_THEN);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_WORD);
    cr_assert_str_eq(tok->data.word, "ok2");
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_OP);
    cr_assert_eq(tok->data.op_data.type, OP_LINEFEED);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_RW);
    cr_assert_eq(tok->data.rw_type, RW_FI);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_EOF);

    lexer_free(lexer); // free vec inside
}
Test(lexer, simple_ionumber)
{
    char *s = "1>";
    struct pretoken_vector *vec = prelexify(s);
    struct lexer *lexer = lexer_new(vec);
    struct token *tok;

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_IONUMBER);
    cr_assert_eq(tok->data.io_number, 1);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_OP);
    cr_assert_eq(tok->data.op_data.type, OP_REDIR);
    cr_assert_eq(tok->data.op_data.data.redir_type, REDIR_GREAT);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_EOF);

    lexer_free(lexer); // free vec inside
}
Test(lexer, simple_ionumber2)
{
    char *s = "8>";
    struct pretoken_vector *vec = prelexify(s);
    struct lexer *lexer = lexer_new(vec);
    struct token *tok;

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_IONUMBER);
    cr_assert_eq(tok->data.io_number, 8);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_OP);
    cr_assert_eq(tok->data.op_data.type, OP_REDIR);
    cr_assert_eq(tok->data.op_data.data.redir_type, REDIR_GREAT);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_EOF);

    lexer_free(lexer); // free vec inside
}
Test(lexer, false_ionumber)
{
    char *s = "88>";
    struct pretoken_vector *vec = prelexify(s);
    struct lexer *lexer = lexer_new(vec);
    struct token *tok;

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_WORD);
    cr_assert_str_eq(tok->data.word, "88");
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_OP);
    cr_assert_eq(tok->data.op_data.type, OP_REDIR);
    cr_assert_eq(tok->data.op_data.data.redir_type, REDIR_GREAT);
    token_free(tok);

    tok = lexer_pop(lexer);
    cr_assert_eq(tok->type, TOKEN_EOF);

    lexer_free(lexer); // free vec inside
}