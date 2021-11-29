#include <err.h>
#include <lexer/lexer.h>
#include <parser/parser.h>
#include <prelexer/prelexer.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc == 1)
        errx(1, "Usage: ./prelexer [SCRIPT]");
    struct pretoken_vector *vec = prelexify(argv[1]);
    struct lexer *lexer = lexer_new(vec);

    struct ast_node *ast = NULL;
    enum parser_status status = parse(&ast, lexer);

    if (status == PARSER_OK)
    {
        printf("PARSER_OK\n");
    }
    else
    {
        printf("PARSER_NOT_OK\n");
    }

    if (ast == NULL)
    {
        printf("ast = (null)\n");
    }
    else
    {
        ast_node_print(ast);
        ast_node_free(ast);
    }

    lexer_free(lexer);
    return 0;
}
