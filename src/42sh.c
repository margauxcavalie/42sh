#include <err.h>
#include <getopt.h>
#include <io/cstream.h>
#include <lexer/lexer.h>
#include <parser/parser.h>
#include <prelexer/prelexer.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utils/vec.h>

/**
 * \brief Parse the command line arguments
 * \return A character stream
 */
static struct cstream *parse_args(int argc, char *argv[], char **to_free,
                                  int *pretty_print)
{
    if (argc >= 2
        && strcmp(argv[1], "--pretty-print") == 0) // temporary it's ugly
    {
        *pretty_print = 1;
        argc -= 1;
    }

    // If launched without argument, read the standard input
    if (argc == 1)
    {
        if (isatty(STDIN_FILENO))
            return cstream_readline_create();
        return cstream_file_create(stdin, /* fclose_on_free */ false);
    }

    // 42sh FILENAME
    if (argc == 2)
    {
        FILE *fp = fopen(argv[1 + *pretty_print], "r");
        if (fp == NULL)
        {
            warn("failed to open input file");
            return NULL;
        }

        return cstream_file_create(fp, /* fclose_on_free */ true);
    }

    if (argc >= 3 && (!(strcmp(argv[1 + *pretty_print], "-c"))))
    {
        char *str =
            zalloc(sizeof(char) * (strlen(argv[2 + *pretty_print]) + 2));
        strcpy(str, argv[2 + *pretty_print]);
        str[strlen(argv[2 + *pretty_print])] = '\n';
        struct cstream *machin = cstream_string_create(str);
        *to_free = str;
        return machin;
    }

    fprintf(stderr, "Usage: %s [OPTIONS] [SCRIPT] [ARGUMENTS]\n", argv[0]);
    return NULL;
}

/**
 * \brief Read and exec until EOF
 * \return An error code
 */
static int execution(struct cstream *cs, struct vec *vec, int pretty_print)
{
    enum error err;
    while (true)
    {
        // Read the next character
        int c;
        if ((err = cstream_pop(cs, &c)))
            return err;

        // If the end of file was reached, add a '\0'
        if (c == EOF)
        {
            vec_push(vec, '\0');
            break;
        }

        vec_push(vec, c);
    }

    // printf("vec->data : %s", vec->data);
    // write(STDOUT_FILENO, vec->data, vec->size - 1);
    struct pretoken_vector *pretoken = prelexify(vec->data);
    if (pretoken == NULL)
        errx(2, "Syntax error");
    vec_reset(vec);
    struct lexer *lexer = lexer_new(pretoken);
    struct ast_node *ast = NULL;
    enum parser_status status = parse(&ast, lexer);

    // printf("parser status = %d\n", status);

    int return_code = 0;
    if (status == PARSER_UNEXPECTED_TOKEN)
    {
        return_code = 2;
    }
    else if (ast == NULL)
    {
        return_code = 0;
    }
    else
    {
        if (pretty_print)
            ast_node_print(ast);
        else
            return_code = ast_node_exec(ast);

        // printf("Exited with status code %d\n", return_code);
        ast_node_free(ast);
    }

    lexer_free(lexer);
    return return_code;
}

int main(int argc, char *argv[])
{
    int rc;
    int pretty_print = 0;
    char *to_free = NULL;

    // Parse command line arguments and get an input stream
    struct cstream *cs;
    if ((cs = parse_args(argc, argv, &to_free, &pretty_print)) == NULL)
    {
        rc = 1;
        goto err_parse_args;
    }

    // Create a vector to hold the current line
    struct vec line;
    vec_init(&line);

    rc = execution(cs, &line, pretty_print); // return code

    // err_loop:
    if (to_free != NULL) // éventuellement faire quelque chose de plus propre
        free(to_free);
    cstream_string_free(cs);
    // cstream_free(cs);
    vec_destroy(&line);
err_parse_args:
    return rc;
}
