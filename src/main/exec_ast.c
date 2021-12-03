#include <err.h>
#include <lexer/lexer.h>
#include <parser/parser.h>
#include <prelexer/prelexer.h>
#include <runtime.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc == 1)
        errx(1, "Usage: ./exec_ast [SCRIPT]");
    struct pretoken_vector *vec = prelexify(argv[1]);
    struct lexer *lexer = lexer_new(vec);

    struct ast_node *ast = NULL;
    enum parser_status status = parse(&ast, &lexer);

    // Runtime struct
    struct runtime *rt = runtime_init();

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
        int return_code = ast_node_exec(ast, rt);
        printf("Exited with status code %d\n", return_code);
        ast_node_free(ast);
    }

    lexer_free(lexer);
    runtime_free(rt);
    return 0;
}
