#include <builtins/builtins.h>
#include <criterion/criterion.h>
#include <criterion/redirect.h>

Test(echo, empty)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "" };
    int res = builtin_echo(2, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("\n");
    runtime_free(rt);
}

Test(echo, one_space)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", " " };
    int res = builtin_echo(2, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str(" \n");
    runtime_free(rt);
}

Test(echo, coucou, .init = cr_redirect_stdout)
{
    struct runtime *rt = runtime_init();
    char *tab[] = { "echo", "coucou" };
    int res = builtin_echo(2, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("coucou\n");
    runtime_free(rt);
}

Test(echo, bateaux_several_args)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "j'adore", "les", "bateaux" };
    int res = builtin_echo(4, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("j'adore les bateaux\n");
    runtime_free(rt);
}

Test(echo, bateaux_one_arg)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "j'adore les bateaux" };
    int res = builtin_echo(2, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("j'adore les bateaux\n");
    runtime_free(rt);
}

Test(echo, one_args_and_several)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "j'apprécie", "les pommes de terre" };
    int res = builtin_echo(3, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("j'apprécie les pommes de terre\n");
    runtime_free(rt);
}

Test(echo, viennoiseries)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "baguette", ", chocolatine, ", "croissant" };
    int res = builtin_echo(4, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("baguette , chocolatine,  croissant\n");
    runtime_free(rt);
}

Test(echo, baguette_backslashs)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "baguette\\" };
    int res = builtin_echo(2, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("baguette\\\n");
    runtime_free(rt);
}

// TESTS WITH '-e' ARG

Test(echo, dash_e_bateau_backslash_n)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-e", "bateau\\n" };
    int res = builtin_echo(3, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("bateau\n\n");
    runtime_free(rt);
}

Test(echo, dash_e_all_types_of_spaces)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-e", "   \\n \\t" };
    int res = builtin_echo(3, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("   \n \t\n");
    runtime_free(rt);
}

Test(echo, dash_e_spaces_and_words)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-e", "42sh c'est\\n", "beaucoup de fun" };
    int res = builtin_echo(4, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("42sh c'est\n beaucoup de fun\n");
    runtime_free(rt);
}

Test(echo, dash_e_colors)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-e", "  rouge\\n", "\\tbleu", "jaune\\t\\n" };
    int res = builtin_echo(5, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("  rouge\n \tbleu jaune\t\n\n");
    runtime_free(rt);
}

Test(echo, dash_e_colors_backslashs)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-e", "  rouge\\", "\\bleu", "jaune\\\\" };
    int res = builtin_echo(5, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("  rouge\\ \\bleu jaune\\\n");
    runtime_free(rt);
}

Test(echo, dash_e_colors_backslashs2)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-e", "  rouge\\\\\\", "\\bleu", "jaune\\\\" };
    int res = builtin_echo(5, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("  rouge\\\\ \\bleu jaune\\\n");
    runtime_free(rt);
}

// TESTS WITH '-n' ARG

Test(echo, dash_n_one_space)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-n", " " };
    int res = builtin_echo(3, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str(" ");
    runtime_free(rt);
}

Test(echo, dash_n_coucou)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-n", "coucou" };
    int res = builtin_echo(3, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("coucou");
    runtime_free(rt);
}

Test(echo, dash_n_bateaux_several_args)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-n", "j'adore", "les", "bateaux" };
    int res = builtin_echo(5, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("j'adore les bateaux");
    runtime_free(rt);
}

Test(echo, dash_n_bateaux_one_arg)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-n", "j'adore les bateaux" };
    int res = builtin_echo(3, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("j'adore les bateaux");
    runtime_free(rt);
}

Test(echo, dash_n_one_args_and_several)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-n", "j'apprécie", "les pommes de terre" };
    int res = builtin_echo(4, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("j'apprécie les pommes de terre");
    runtime_free(rt);
}

Test(echo, dash_n_viennoiseries)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-n", "baguette", ", chocolatine, ", "croissant" };
    int res = builtin_echo(5, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("baguette , chocolatine,  croissant");
    runtime_free(rt);
}

// TESTS WITH '-ne' ARG

Test(echo, dash_ne_bateau_backslash_n)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-ne", "bateau\\n" };
    int res = builtin_echo(3, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("bateau\n");
    runtime_free(rt);
}

Test(echo, dash_ne_all_types_of_spaces)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-ne", "   \\n \\t" };
    int res = builtin_echo(3, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("   \n \t");
    runtime_free(rt);
}

Test(echo, dash_ne_spaces_and_words)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-ne", "42sh c'est\\n", "beaucoup de fun" };
    int res = builtin_echo(4, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("42sh c'est\n beaucoup de fun");
    runtime_free(rt);
}

Test(echo, dash_ne_colors)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-ne", "  rouge\\n", "\\tbleu", "jaune\\t\\n" };
    int res = builtin_echo(5, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("  rouge\n \tbleu jaune\t\n");
    runtime_free(rt);
}

Test(echo, dash_ne_viennoiseries)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-ne", "baguette\\n", ", \\tchocolatine, ",
                    "croissant" };
    int res = builtin_echo(5, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("baguette\n , \tchocolatine,  croissant");
    runtime_free(rt);
}

Test(echo, dash_ne_colors_backslashs)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-ne", "  rouge\\", "\\bleu", "jaune\\\\" };
    int res = builtin_echo(5, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("  rouge\\ \\bleu jaune\\");
    runtime_free(rt);
}

Test(echo, dash_ne_colors_backslashs2)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-ne", "  rouge\\\\\\", "\\bleu", "jaune\\\\" };
    int res = builtin_echo(5, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("  rouge\\\\ \\bleu jaune\\");
    runtime_free(rt);
}

// TESTS WITH '-en' ARG

Test(echo, dash_en_bateau_backslash_n)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-en", "bateau\\n" };
    int res = builtin_echo(3, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("bateau\n");
    runtime_free(rt);
}

Test(echo, dash_en_all_types_of_spaces)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-en", "   \\n \\t" };
    int res = builtin_echo(3, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("   \n \t");
    runtime_free(rt);
}

Test(echo, dash_en_spaces_and_words)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-en", "42sh c'est\\n", "beaucoup de fun" };
    int res = builtin_echo(4, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("42sh c'est\n beaucoup de fun");
    runtime_free(rt);
}

Test(echo, dash_en_colors)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-en", "  rouge\\n", "\\tbleu", "jaune\\t\\n" };
    int res = builtin_echo(5, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("  rouge\n \tbleu jaune\t\n");
    runtime_free(rt);
}

Test(echo, dash_en_viennoiseries)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-en", "baguette\\n", ", \\tchocolatine, ",
                    "croissant" };
    int res = builtin_echo(5, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("baguette\n , \tchocolatine,  croissant");
    runtime_free(rt);
}

Test(echo, dash_en_viennoiseries_backslashs)
{
    struct runtime *rt = runtime_init();
    cr_redirect_stdout();
    char *tab[] = { "echo", "-en", "baguette\\n\\\\\\", ", \\tchocolatine, ",
                    "croissant\\\\" };
    int res = builtin_echo(5, tab, rt);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("baguette\n\\\\ , \tchocolatine,  croissant\\");
    runtime_free(rt);
}
