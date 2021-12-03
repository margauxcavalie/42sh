#include <criterion/criterion.h>
#include <prelexer/prelexer.h>
#include <stdio.h>
#include <string.h>

/*
 * Tests the normal functioning of the prelexer
 */

Test(prelexify, quotes_empty)
{
    char *input = "  '  hello   '  ";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(strcmp(vec->data[0]->str, "  hello   "), 0);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[0]->is_quoted, 1);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_two_words)
{
    char *input = "  '  hello   '   'hi  '  ";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 3);
    cr_assert_eq(strcmp(vec->data[0]->str, "  hello   "), 0);
    cr_assert_eq(strcmp(vec->data[1]->str, "hi  "), 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[0]->is_quoted, 1);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->is_quoted, 1);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_op)
{
    char *input = "  '  hello  ;;\n ';   'hi  '  ";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 4);
    cr_assert_eq(strcmp(vec->data[0]->str, "  hello  ;;\n "), 0);
    cr_assert_eq(strcmp(vec->data[2]->str, "hi  "), 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[0]->is_quoted, 1);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[2]->is_quoted, 1);
    cr_assert_eq(vec->data[3]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_strange_word)
{
    char *input = "hjuzegfajh'      'sdgqsdgsd";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(strcmp(vec->data[0]->str, "hjuzegfajh      sdgqsdgsd"), 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[0]->is_quoted, 1);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_strange_word2)
{
    char *input = "hjuzegfajh'      'sdgqsdgsd';'''''';;;;'";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(strcmp(vec->data[0]->str, "hjuzegfajh      sdgqsdgsd;;;;;"),
                 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[0]->is_quoted, 1);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_no_quotes)
{
    char *input = "yo 'yo'";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 3);
    cr_assert_eq(strcmp(vec->data[0]->str, "yo"), 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[0]->is_quoted, 0);
    cr_assert_eq(strcmp(vec->data[1]->str, "yo"), 0);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->is_quoted, 1);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_fail)
{
    char *input = " '   hello'  'couille";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec, NULL);
}

Test(prelexify, dblquotes_fail)
{
    char *input = " \"   hello\"  \"couille";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec, NULL);
}

Test(prelexify, dblquotes_two_words)
{
    char *input = "  \"  hello   \"   \"hi  \"  ";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 3);
    cr_assert_eq(strcmp(vec->data[0]->str, "  hello   "), 0);
    cr_assert_eq(strcmp(vec->data[1]->str, "hi  "), 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[0]->is_quoted, 2);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->is_quoted, 2);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, dblquotes_in_quotes)
{
    char *input = "  \"  hello   '   'hi  \"  ";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(strcmp(vec->data[0]->str, "  hello   '   'hi  "), 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[0]->is_quoted, 2);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_in_dblquotes)
{
    char *input = "  '  hello   \"   \"hi  '  ";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(strcmp(vec->data[0]->str, "  hello   \"   \"hi  "), 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[0]->is_quoted, 1);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, backslash)
{
    char *input = "\\h\\e\\l\\l\\o";
    struct pretoken_vector *vec = prelexify(input);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(strcmp(vec->data[0]->str, "hello"), 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[0]->is_quoted, 1);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}
