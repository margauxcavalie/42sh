#include <criterion/criterion.h>
#include <prelexer/prelexer.h>

/*
 * Tests the normal functioning of the prelexer
 */

Test(prelexify, empty)
{
    int i = 0;
    char *input = "";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 1);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, just_spaces)
{
    int i = 0;
    char *input = "    \t";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 1);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, just_word)
{
    int i = 0;
    char *input = "    word\t";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, just_word_if)
{
    int i = 0;
    char *input = "    if\t";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, just_word_semicolon)
{
    int i = 0;
    char *input = "    ;\t";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, just_3_word)
{
    int i = 0;
    char *input = "  pierre <3 lucas  \t";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 5);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[3]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[4]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, hard_word)
{
    int i = 0;
    char *input = "bon\rj\vour";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, mix_types1)
{
    int i = 0;
    char *input = " \t pomme ; poire  \t";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 4);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[3]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, mix_types2)
{
    int i = 0;
    char *input = " \t; pomme ; ; poire  \t";
    struct pretoken_vector *vec = prelexify(input, &i);
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
    int i = 0;
    char *input = "for do in salut;";
    struct pretoken_vector *vec = prelexify(input, &i);
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
    int i = 0;
    char *input = "word;";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 3);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, word_linefeed)
{
    int i = 0;
    char *input = "word\n";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 3);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, word_semicolon_word)
{
    int i = 0;
    char *input = "word;word";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 4);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[3]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, one_pipe)
{
    int i = 0;
    char *input = "|";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, word_pipe_word)
{
    int i = 0;
    char *input = "word|word";
    struct pretoken_vector *vec = prelexify(input, &i);
    free_pretoken_list(vec);
}

Test(prelexify, word_pipe_word_spaces)
{
    int i = 0;
    char *input = "word | word";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 4);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[3]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, double_pipe_between_word)
{
    int i = 0;
    char *input = "word | | word";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 5);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[3]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[4]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}
