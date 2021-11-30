#include <criterion/criterion.h>
#include <prelexer/prelexer.h>
#include <stdio.h>

/*
 * Tests the normal functioning of the prelexer
 */

Test(prelexify, quotes_empty)
{
    char *input = "  '  hello   '  ";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_dbl)
{
    char *input = "  '  hello   '   'hi  '  ";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 3);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_op)
{
    char *input = "  '  hello  ;;\n ';   'hi  '  ";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 4);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[3]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_strange_word)
{
    char *input = "hjuzegfajh'      'sdgqsdgsd";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_strange_word2)
{
    char *input = "hjuzegfajh'      'sdgqsdgsd';'''''';;;;'";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_fail)
{
    char *input = " '   hello'  'couille";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec, NULL);
}
