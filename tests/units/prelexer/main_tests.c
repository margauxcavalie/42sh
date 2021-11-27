#include <criterion/criterion.h>
#include <prelexer/prelexer.h>

/*
 * Tests the normal functioning of the prelexer
 */

Test(prelexify, empty)
{
    char *input = "";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 1);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, just_spaces)
{
    char *input = "    \t";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 1);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, just_word)
{
    char *input = "    word\t";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, just_word_if)
{
    char *input = "    if\t";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, just_word_semicolon)
{
    char *input = "    ;\t";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, just_3_word)
{
    char *input = "  pierre <3 lucas  \t";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 4);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[3]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, hard_word)
{
    char *input = "bon\rj\vour";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, mix_types1)
{
    char *input = " \t pomme ; poire  \t";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 4);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[3]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, mix_types2)
{
    char *input = " \t; pomme ; ; poire  \t";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 6);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[3]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[4]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[5]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, hard_example)
{
    char *input = "for do in salut;";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 6);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[3]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[4]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[5]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}


Test(prelexify, word_semicolon)
{
    char *input = "word;";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 3);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}


Test(prelexify, word_linefeed)
{
    char *input = "word\n";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 3);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}


Test(prelexify, word_semicolon_word)
{
    char *input = "word;word";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 4);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[3]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

