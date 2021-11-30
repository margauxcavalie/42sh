#include <builtins/builtins.h>
#include <criterion/criterion.h>
#include <criterion/redirect.h>

Test(echo, empty)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "" };
    int res = builtin_echo(2, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("\n");
}

Test(echo, one_space)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", " " };
    int res = builtin_echo(2, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str(" \n");
}

Test(echo, coucou, .init = cr_redirect_stdout)
{
    char *tab[] = { "echo", "coucou" };
    int res = builtin_echo(2, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("coucou\n");
}

Test(echo, bateaux_several_args)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "j'adore", "les", "bateaux" };
    int res = builtin_echo(4, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("j'adore les bateaux\n");
}

Test(echo, bateaux_one_arg)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "j'adore les bateaux" };
    int res = builtin_echo(2, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("j'adore les bateaux\n");
}

Test(echo, one_args_and_several)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "j'apprécie", "les pommes de terre" };
    int res = builtin_echo(3, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("j'apprécie les pommes de terre\n");
}

Test(echo, viennoiseries)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "baguette", ", chocolatine, ", "croissant" };
    int res = builtin_echo(4, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("baguette , chocolatine,  croissant\n");
}

Test(echo, baguette_backslashs)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "baguette\\" };
    int res = builtin_echo(2, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("baguette\\\n");
}

// TESTS WITH '-e' ARG

Test(echo, dash_e_bateau_backslash_n)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-e", "bateau\\n" };
    int res = builtin_echo(3, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("bateau\n\n");
}

Test(echo, dash_e_all_types_of_spaces)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-e", "   \\n \\t" };
    int res = builtin_echo(3, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("   \n \t\n");
}

Test(echo, dash_e_spaces_and_words)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-e", "42sh c'est\\n", "beaucoup de fun" };
    int res = builtin_echo(4, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("42sh c'est\n beaucoup de fun\n");
}

Test(echo, dash_e_colors)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-e", "  rouge\\n", "\\tbleu", "jaune\\t\\n" };
    int res = builtin_echo(5, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("  rouge\n \tbleu jaune\t\n\n");
}

Test(echo, dash_e_colors_backslashs)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-e", "  rouge\\", "\\bleu", "jaune\\\\" };
    int res = builtin_echo(5, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("  rouge\\ \\bleu jaune\\\n");
}

Test(echo, dash_e_colors_backslashs2)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-e", "  rouge\\\\\\", "\\bleu", "jaune\\\\" };
    int res = builtin_echo(5, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("  rouge\\\\ \\bleu jaune\\\n");
}

// TESTS WITH '-n' ARG

Test(echo, dash_n_empty)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-n", "" };
    int res = builtin_echo(3, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("");
}

Test(echo, dash_n_one_space)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-n", " " };
    int res = builtin_echo(3, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str(" ");
}

Test(echo, dash_n_coucou)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-n", "coucou" };
    int res = builtin_echo(3, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("coucou");
}

Test(echo, dash_n_bateaux_several_args)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-n", "j'adore", "les", "bateaux" };
    int res = builtin_echo(5, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("j'adore les bateaux");
}

Test(echo, dash_n_bateaux_one_arg)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-n", "j'adore les bateaux" };
    int res = builtin_echo(3, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("j'adore les bateaux");
}

Test(echo, dash_n_one_args_and_several)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-n", "j'apprécie", "les pommes de terre" };
    int res = builtin_echo(4, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("j'apprécie les pommes de terre");
}

Test(echo, dash_n_viennoiseries)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-n", "baguette", ", chocolatine, ", "croissant" };
    int res = builtin_echo(5, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("baguette , chocolatine,  croissant");
}

// TESTS WITH '-ne' ARG

Test(echo, dash_ne_bateau_backslash_n)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-ne", "bateau\\n" };
    int res = builtin_echo(3, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("bateau\n");
}

Test(echo, dash_ne_all_types_of_spaces)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-ne", "   \\n \\t" };
    int res = builtin_echo(3, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("   \n \t");
}

Test(echo, dash_ne_spaces_and_words)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-ne", "42sh c'est\\n", "beaucoup de fun" };
    int res = builtin_echo(4, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("42sh c'est\n beaucoup de fun");
}

Test(echo, dash_ne_colors)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-ne", "  rouge\\n", "\\tbleu", "jaune\\t\\n" };
    int res = builtin_echo(5, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("  rouge\n \tbleu jaune\t\n");
}

Test(echo, dash_ne_viennoiseries)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-ne", "baguette\\n", ", \\tchocolatine, ",
                    "croissant" };
    int res = builtin_echo(5, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("baguette\n , \tchocolatine,  croissant");
}

Test(echo, dash_ne_colors_backslashs)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-ne", "  rouge\\", "\\bleu", "jaune\\\\" };
    int res = builtin_echo(5, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("  rouge\\ \\bleu jaune\\");
}

Test(echo, dash_ne_colors_backslashs2)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-ne", "  rouge\\\\\\", "\\bleu", "jaune\\\\" };
    int res = builtin_echo(5, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("  rouge\\\\ \\bleu jaune\\");
}

// TESTS WITH '-en' ARG

Test(echo, dash_en_bateau_backslash_n)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-en", "bateau\\n" };
    int res = builtin_echo(3, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("bateau\n");
}

Test(echo, dash_en_all_types_of_spaces)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-en", "   \\n \\t" };
    int res = builtin_echo(3, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("   \n \t");
}

Test(echo, dash_en_spaces_and_words)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-en", "42sh c'est\\n", "beaucoup de fun" };
    int res = builtin_echo(4, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("42sh c'est\n beaucoup de fun");
}

Test(echo, dash_en_colors)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-en", "  rouge\\n", "\\tbleu", "jaune\\t\\n" };
    int res = builtin_echo(5, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("  rouge\n \tbleu jaune\t\n");
}

Test(echo, dash_en_viennoiseries)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-en", "baguette\\n", ", \\tchocolatine, ",
                    "croissant" };
    int res = builtin_echo(5, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("baguette\n , \tchocolatine,  croissant");
}

Test(echo, dash_en_viennoiseries_backslashs)
{
    cr_redirect_stdout();
    char *tab[] = { "echo", "-en", "baguette\\n\\\\\\", ", \\tchocolatine, ",
                    "croissant\\\\" };
    int res = builtin_echo(5, tab);
    fflush(stdout);
    cr_assert_eq(res, 0);
    cr_assert_stdout_eq_str("baguette\n\\\\ , \tchocolatine,  croissant\\");
}
