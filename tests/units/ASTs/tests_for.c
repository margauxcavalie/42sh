#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <parser/for/ast_for.h>
#include <parser/simple_cmd/ast_simple_cmd.h>
#include <vector/vector.h>

Test(ast_for, empty)
{
    // Runtime struct
    struct runtime *rt = runtime_init();
    // init an empty AST while
    struct ast_for *ast = ast_for_init();
    struct print_context pc = { 0 };

    // test print
    cr_redirect_stdout();
    (*(ast->base.node_print))((struct ast_node *)ast, pc);
    fflush(stdout);
    cr_assert_stdout_eq_str("for { (null) } in { };\ndo {\n\n};");

    // test exec
    int res = (*(ast->base.node_exec))((struct ast_node *)ast, rt);
    cr_assert_eq(res, 0);

    // free
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
    runtime_free(rt);
}

Test(ast_for, print_test_concret)
{
    // Runtime struct
    struct runtime *rt = runtime_init();
    // init an empty AST while
    struct ast_for *ast = ast_for_init();
    ast_for_set_var_name(ast, "elt");

    struct ast_simple_cmd *body = ast_simple_cmd_init();
    ast_simple_cmd_add_param(body, "echo");
    ast_simple_cmd_add_param(body, "teckel");
    ast_for_set_body(ast, &(body->base));

    ast_for_add_word(ast, "coucou");
    ast_for_add_word(ast, "salut");

    // test print
    cr_redirect_stdout();
    struct print_context pc = { 0 };
    (*(ast->base.node_print))((struct ast_node *)ast, pc);
    fflush(stdout);
    cr_assert_stdout_eq_str(
        "for { elt } in { coucou salut };\ndo {\n    echo teckel\n};");

    // free
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
    runtime_free(rt);
}

Test(ast_for, exec_test_concret)
{
    // Runtime struct
    struct runtime *rt = runtime_init();
    // init an empty AST while
    struct ast_for *ast = ast_for_init();
    ast_for_set_var_name(ast, "elt");

    struct ast_simple_cmd *body = ast_simple_cmd_init();
    ast_simple_cmd_add_param(body, "echo");
    ast_simple_cmd_add_param(body, "saint");
    ast_simple_cmd_add_param(body, "bernard");
    ast_for_set_body(ast, &(body->base));

    ast_for_add_word(ast, "1");
    ast_for_add_word(ast, "2");
    ast_for_add_word(ast, "3");
    ast_for_add_word(ast, "4");
    ast_for_add_word(ast, "5");
    ast_for_add_word(ast, "6");

    // test exec
    cr_redirect_stdout();
    (*(ast->base.node_exec))((struct ast_node *)ast, rt);
    fflush(stdout);
    cr_assert_stdout_eq_str("saint bernard\nsaint bernard\nsaint bernard\nsaint bernard\nsaint bernard\nsaint bernard\n");

    // free
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
    runtime_free(rt);
}
