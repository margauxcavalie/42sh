#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <parser/simple_cmd/ast_simple_cmd.h>
#include <vector/vector.h>

Test(AST, print_vegetables)
{
    cr_redirect_stdout();
    // INITIALIZATION
    struct ast_simple_cmd *ast = ast_simple_cmd_init();
    cr_assert_eq(ast->base.type, AST_SIMPLE_CMD);
    cr_assert_eq(5, ast->params->capacity);
    cr_assert_eq(0, ast->params->size);
    struct print_context pc = { 0 };
    (*(ast->base.node_print))((struct ast_node *)ast, pc);
    int res = (*(ast->base.node_exec))((struct ast_node *)ast);
    cr_assert_eq(res, 0);
    ////

    // ADD PARAMETERS
    ast = ast_simple_cmd_add_param(ast, "patate");
    cr_assert_str_eq(ast->params->data[0], "patate");
    cr_assert_eq(1, ast->params->size);
    ast = ast_simple_cmd_add_param(ast, "radis");
    ast = ast_simple_cmd_add_param(ast, "tomate");
    ast = ast_simple_cmd_add_param(ast, "poireau");
    ast = ast_simple_cmd_add_param(ast, "courgette");
    ast = ast_simple_cmd_add_param(ast, "navet");
    ast = ast_simple_cmd_add_param(ast, "brocoli");
    cr_assert_str_eq(ast->params->data[3], "poireau");
    cr_assert_eq(7, ast->params->size);
    ////

    // PRINT
    (*(ast->base.node_print))((struct ast_node *)ast, pc);
    fflush(stdout);
    cr_assert_stdout_eq_str(
        "patate radis tomate poireau courgette navet brocoli");

    // FREE
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
}

Test(AST, execution_vegetables)
{
    cr_redirect_stdout();
    struct ast_simple_cmd *ast = ast_simple_cmd_init();
    ast = ast_simple_cmd_add_param(ast, "echo");
    ast = ast_simple_cmd_add_param(ast, "tomate");
    ast = ast_simple_cmd_add_param(ast, "poireau");
    ast = ast_simple_cmd_add_param(ast, "courgette");
    ast = ast_simple_cmd_add_param(ast, "navet");
    ast = ast_simple_cmd_add_param(ast, "brocoli");
    ast = ast_simple_cmd_add_param(ast, "echo");
    ast = ast_simple_cmd_add_param(ast, "fenouil");
    (*(ast->base.node_exec))((struct ast_node *)ast);
    fflush(stdout);
    cr_assert_stdout_eq_str(
        "tomate poireau courgette navet brocoli echo fenouil\n");

    cr_assert_eq(8, ast->params->size);
    // FREE
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
}

Test(AST, execution_unknown_command)
{
    cr_redirect_stderr(); // not useful, just to avoid seeing the error msg
    struct ast_simple_cmd *ast = ast_simple_cmd_init();
    ast = ast_simple_cmd_add_param(ast, "patate");
    ast = ast_simple_cmd_add_param(ast, "tomate");
    ast = ast_simple_cmd_add_param(ast, "poireau");
    ast = ast_simple_cmd_add_param(ast, "courgette");
    ast = ast_simple_cmd_add_param(ast, "navet");
    ast = ast_simple_cmd_add_param(ast, "brocoli");
    ast = ast_simple_cmd_add_param(ast, "echo");
    ast = ast_simple_cmd_add_param(ast, "fenouil");
    int res = (*(ast->base.node_exec))((struct ast_node *)ast);
    cr_assert_eq(127, res);

    // FREE
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
}
