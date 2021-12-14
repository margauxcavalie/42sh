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
    int prelexify_continue = 0;
    struct pretoken_vector *pretoken = prelexify(str, &prelexify_continue);
    if (end == false && prelexify_continue)
    {
        free_pretoken_list(pretoken);
        return 1;
    }

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
    int prev = 0;
    while (true)
    {
        // Read the next character
        int c;
        if ((err = cstream_pop(cs, &c)))
            return err;
        // If the end of file was reached, stop right there
        if (c == EOF)
            break;

        // if /n is escaped
        if (prev % 2 == 1 && c == '\n')
        {
            line->size -= 1;
            prev = 0;
            continue;
        }

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
            prev = c;
            continue;
        }

        // Otherwise, add the character to the line
        vec_push(line, c);
        if (c == '\\')
            prev += 1;
        else
            prev = 0;
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

static void set_arg_var(struct runtime *rt, int argc, char *argv[], int ind)
{
    // set IFS
    char *ifs = zalloc(sizeof(char) * 4);
    strcpy(ifs, "IFS");
    char *value = zalloc(sizeof(char) * 4);
    strcpy(value, "");
    var_hash_map_insert(rt->variables, ifs, value);
    // set args
    int i = 0;
    struct vec *vec = xmalloc(sizeof(struct vec));
    vec_init(vec);
    for (; ind < argc; ind++)
    {
        // add to de $1 ... $n list
        char *arg = zalloc(sizeof(char) * 10);
        sprintf(arg, "%d", i);
        char *arg_value = zalloc(sizeof(char) * (strlen(argv[ind]) + 1));
        sprintf(arg_value, "%s", argv[ind]);
        var_hash_map_insert(rt->variables, arg, arg_value);
        // set $* and $@
        if (i != 0)
        {
            for (size_t n = 0; n < strlen(arg_value); n++)
            {
                vec_push(vec, arg_value[n]);
            }
            if (ind + 1 != argc)
                vec_push(vec, ' ');
        }
        i++;
    }
    // set $* and $@
    char *arg = zalloc(sizeof(char) * 2);
    sprintf(arg, "*");
    char *arg_value = zalloc(sizeof(char) * (vec->size + 1));
    char *str = vec_cstring(vec);
    sprintf(arg_value, "%s", vec_cstring(vec));
    var_hash_map_insert(rt->variables, arg, arg_value);
    arg = zalloc(sizeof(char) * 2);
    sprintf(arg, "@");
    var_hash_map_insert(rt->variables, arg, str);
    free(vec);
    // set $#
    arg = zalloc(sizeof(char) * 2);
    sprintf(arg, "#");
    arg_value = zalloc(sizeof(char) * 10);
    sprintf(arg_value, "%d", i - 1);
    var_hash_map_insert(rt->variables, arg, arg_value);
    // set PWD et OLDPWD
    char *env_pwd = getenv("PWD");
    char *env_old_pwd = getenv("OLDPWD");
    // PWD
    if (env_pwd != NULL)
    {
        arg = zalloc(sizeof(char) * 4);
        sprintf(arg, "PWD");
        arg_value = zalloc(sizeof(char) * (strlen(env_pwd) + 1));
        sprintf(arg_value, "%s", env_pwd);
        var_hash_map_insert(rt->variables, arg, arg_value);
    }
    // OLDPWD
    if (env_old_pwd != NULL)
    {
        arg = zalloc(sizeof(char) * 7);
        sprintf(arg, "OLDPWD");
        arg_value = zalloc(sizeof(char) * (strlen(env_old_pwd) + 1));
        sprintf(arg_value, "%s", env_old_pwd);
        var_hash_map_insert(rt->variables, arg, arg_value);
    }
    // UID
    char *env_uid = getenv("UID");
    if (env_uid != NULL)
    {
        arg = zalloc(sizeof(char) * 4);
        sprintf(arg, "UID");
        arg_value = zalloc(sizeof(char) * (strlen(env_uid) + 1));
        sprintf(arg_value, "%s", env_uid);
        var_hash_map_insert(rt->variables, arg, arg_value);
    }
}

int main(int argc, char *argv[])
{
    // Runtime initialisation
    struct runtime *rt = runtime_init();
    int rc;

    struct opts opts = get_options(argc, argv);

    // Setup the script argument
    set_arg_var(rt, argc, argv, opts.ind_script_args);

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
