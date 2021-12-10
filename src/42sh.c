#include <err.h>
#include <io/cstream.h>
#include <lexer/lexer.h>
#include <options.h>
#include <parser/parser.h>
#include <prelexer/prelexer.h>
#include <runtime.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utils/vec.h>

/**
 * \brief Parse the command line arguments
 * \return A character stream
 */
static struct cstream *parse_args(struct opts opts)
{
    // If launched without argument, read the standard input
    if (opts.type == INPUT_PROMPT)
    {
        if (isatty(STDIN_FILENO))
            return cstream_readline_create();
        return cstream_file_create(stdin, /* fclose_on_free */ false);
    }

    // 42sh FILENAME
    if (opts.type == INPUT_FILE)
    {
        FILE *fp = fopen(opts.path, "r");
        if (fp == NULL)
        {
            warn("failed to open input file");
            return NULL;
        }

        return cstream_file_create(fp, /* fclose_on_free */ true);
    }

    if (opts.type == INPUT_STRING)
    {
        return cstream_string_create(opts.cmd);
    }
    return NULL; // never executed
}

static void print_parser_error(enum parser_status status)
{
    if (status == PARSER_UNEXPECTED_TOKEN)
        warnx("Syntax error: Unexpected token");
    else if (status == PARSER_UNTERMINATED_QUOTES)
        warnx("Syntax error: Unterminated quoted string");
    else if (status == PARSER_UNEXPECTED_EOF)
        warnx("Syntax error: end of file unexpected");
    else
        warnx("Syntax error: unknown error");
}

static int try_parse(struct ast_node **ast, struct vec *line, bool end)
{
    const char *str = vec_cstring(line);
    struct pretoken_vector *pretoken = prelexify(str);

    struct lexer *lexer = lexer_new(pretoken);
    enum parser_status status = parse(ast, &lexer);
    lexer_free(lexer);

    if (status == PARSER_OK)
        return 0;
    if (end == false && status == PARSER_UNEXPECTED_EOF)
        return 1;
    print_parser_error(status);
    return 2;
}

static void execution(struct ast_node *ast, struct runtime *rt,
                      struct opts opts)
{
    int return_code = 0;

    if (opts.pretty_print)
    {
        ast_node_print(ast);
    }
    else
    {
        return_code = ast_node_exec(ast, rt);
    }
    // set the last return code
    runtime_set_status(rt, return_code);
}

/**
 * \brief Read and print lines on newlines until EOF
 * \return An error code
 */
enum error read_print_loop(struct cstream *cs, struct vec *line,
                           struct runtime *rt, struct opts opts)
{
    enum error err;

    const struct cstream_type *type = cs->type;

    while (true)
    {
        // Read the next character
        int c;
        if ((err = cstream_pop(cs, &c)))
            return err;

        // If the end of file was reached, stop right there
        if (c == EOF)
            break;

        // If a newline was met, print the line
        if (c == '\n')
        {
            struct ast_node *ast = NULL;
            int e = try_parse(&ast, line, false);
            if (e == 0) // PARSER_OK
            {
                execution(ast, rt, opts);
                vec_reset(line);
                ast_node_free(ast);
                if (type->interactive)
                    type->reset(cs);
                if (rt->encountered_exit == true)
                    break;
            }
            else if (e == 1)
            {
                vec_push(line, c);
            }
            else if (e == 2)
            {
                runtime_set_status(rt, 2);
                vec_reset(line);
                if (!type->interactive)
                    break;
                else
                    type->reset(cs);
            }
            continue;
        }

        // Otherwise, add the character to the line
        vec_push(line, c);
    }

    if (line->size > 0)
    {
        struct ast_node *ast = NULL;
        int e = try_parse(&ast, line, true);
        if (e == 0) // PARSER_OK
            execution(ast, rt, opts);
        else if (e == 2)
            runtime_set_status(rt, 2);
        ast_node_free(ast);
    }

    return NO_ERROR;
}

int main(int argc, char *argv[])
{
    // Runtime initialisation
    struct runtime *rt = runtime_init();
    int rc;

    struct opts opts = get_options(argc, argv);

    // Parse command line arguments and get an input stream
    struct cstream *cs;
    if ((cs = parse_args(opts)) == NULL)
    {
        runtime_set_status(rt, 1);
        goto err_parse_args;
    }

    // Create a vector to hold the current line
    struct vec line;
    vec_init(&line);

    // Run the test loop
    if (read_print_loop(cs, &line, rt, opts) != NO_ERROR)
    {
        runtime_set_status(rt, 1);
        goto err_loop;
    }

err_loop:
    cstream_free(cs);
    vec_destroy(&line);
err_parse_args:
    rc = rt->last_status;
    runtime_free(rt);
    return rc;
}
