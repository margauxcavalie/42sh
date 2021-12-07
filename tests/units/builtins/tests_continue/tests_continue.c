#include <builtins/builtins.h>
#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <parser/cmd_list/ast_cmd_list.h>
#include <parser/for/ast_for.h>
#include <parser/simple_cmd/ast_simple_cmd.h>

// depends on AST for
Test(test_continue, without_args)
{
    struct runtime *rt = runtime_init();
    struct ast_for *ast = ast_for_init();
    ast_for_set_var_name(ast, "elt");
    ast_for_add_word(ast, "1");
    ast_for_add_word(ast, "2");
    ast_for_add_word(ast, "3");

    struct ast_simple_cmd *body = ast_simple_cmd_init();
    ast_simple_cmd_add_param(body, "echo");
    ast_simple_cmd_add_param(body, "coucou");

    struct ast_simple_cmd *body2 = ast_simple_cmd_init();
    ast_simple_cmd_add_param(body2, "continue");

    struct ast_simple_cmd *body3 = ast_simple_cmd_init();
    ast_simple_cmd_add_param(body3, "echo");
    ast_simple_cmd_add_param(body3, "salut");

    struct ast_cmd_list *list = ast_cmd_list_init();
    ast_cmd_list_add_ast(list, &(body->base));
    ast_cmd_list_add_ast(list, &(body2->base));
    ast_cmd_list_add_ast(list, &(body3->base));

    ast_for_set_body(ast, &(list->base));

    // test exec
    cr_redirect_stdout();
    (*(ast->base.node_exec))((struct ast_node *)ast, rt);
    fflush(stdout);
    cr_assert_stdout_eq_str("coucou\ncoucou\ncoucou\n");

    // free
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
    runtime_free(rt);
}
