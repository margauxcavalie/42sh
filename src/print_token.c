#include <lexer/lexer.h>
#include <prelexer/prelexer.h>

#include <stdio.h>
#include <err.h>

int main(int argc, char *argv[])
{
    if (argc == 1)
        errx(1,"Usage: ./prelexer [SCRIPT]");
    struct pretoken_vector *vec = prelexify(argv[1]);
    struct lexer *lexer = lexer_new(vec);
    struct token *token = lexer_pop(lexer);

    while (token->type != TOKEN_EOF)
    {
        if (token->type == TOKEN_WORD)
            printf("W ");
        if (token->type == TOKEN_RW)
            printf("RW ");
        if (token->type == TOKEN_OP)
            printf("OP ");
        token_free(token);
        token = lexer_pop(lexer);
    }
    printf("EOF\n");
    token_free(token);
    lexer_free(lexer);
    return 0;
}

