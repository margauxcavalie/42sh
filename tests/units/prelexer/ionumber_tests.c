#include <criterion/criterion.h>
#include <prelexer/prelexer.h>
#include <stdio.h>
#include <string.h>

/*
 * Tests the normal functioning of the prelexer
 */

Test(prelexify, simple_ionumber)
{
    char *input = "1>";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 3);

    cr_assert_str_eq(vec->data[0]->str, "1");
    cr_assert_str_eq(vec->data[1]->str, ">");

    cr_assert_eq(vec->data[0]->type, PRETOKEN_IONUMBER);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_EOF);

    free_pretoken_list(vec);
}

Test(prelexify, simple_ionumber2)
{
    char *input = "1<";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 3);

    cr_assert_str_eq(vec->data[0]->str, "1");
    cr_assert_str_eq(vec->data[1]->str, "<");

    cr_assert_eq(vec->data[0]->type, PRETOKEN_IONUMBER);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_EOF);

    free_pretoken_list(vec);
}

Test(prelexify, false_ionumber1)
{
    char *input = "11>";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 3);

    cr_assert_str_eq(vec->data[0]->str, "11");
    cr_assert_str_eq(vec->data[1]->str, ">");

    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_EOF);

    free_pretoken_list(vec);
}

Test(prelexify, simple_ionumber_but_quoted)
{
    char *input = "''1>";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 3);

    cr_assert_str_eq(vec->data[0]->str, "1");
    cr_assert_str_eq(vec->data[1]->str, ">");

    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_EOF);

    free_pretoken_list(vec);
}

Test(prelexify, simple_ionumber_but_with_space)
{
    char *input = "1 >";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 3);

    cr_assert_str_eq(vec->data[0]->str, "1");
    cr_assert_str_eq(vec->data[1]->str, ">");

    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_EOF);

    free_pretoken_list(vec);
}
