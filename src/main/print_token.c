#include <err.h>
#include <lexer/lexer.h>
#include <prelexer/prelexer.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc == 1)
        errx(1, "Usage: ./prelexer [SCRIPT]");
    int i = 0;
    struct pretoken_vector *vec = prelexify(argv[1], &i);
    if (vec == NULL)
        errx(2, "Syntax error");
    struct lexer *lexer = lexer_new(vec);
    struct token *token = lexer_pop(lexer);

    while (token->type != TOKEN_EOF)
    {
        if (token->type == TOKEN_WORD)
        {
            printf("W ");
        }
        if (token->type == TOKEN_RW)
            printf("RW ");
        if (token->type == TOKEN_OP)
            printf("OP ");
        if (token->type == TOKEN_IONUMBER)
            printf("IO ");
        if (token->type == TOKEN_ASSIGNEMENT_WORD)
            printf("AW ");
        token_free(token);
        token = lexer_pop(lexer);
    }
    printf("EOF\n");
    lexer_free(lexer);
    return 0;
}
