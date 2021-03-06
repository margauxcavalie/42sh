#include <criterion/criterion.h>
#include <prelexer/prelexer.h>
#include <stdio.h>
#include <string.h>

/*
 * Tests the normal functioning of the prelexer
 */

Test(prelexify, quotes_empty)
{
    int i = 0;
    char *input = "  '  hello   '  ";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(strcmp(vec->data[0]->str, "'  hello   '"), 0);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_two_words)
{
    int i = 0;
    char *input = "  '  hello   '   'hi  '  ";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 3);
    cr_assert_eq(strcmp(vec->data[0]->str, "'  hello   '"), 0);
    cr_assert_eq(strcmp(vec->data[1]->str, "'hi  '"), 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_op)
{
    int i = 0;
    char *input = "  '  hello  ;;\n ';   'hi  '  ";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 4);
    cr_assert_eq(strcmp(vec->data[0]->str, "'  hello  ;;\n '"), 0);
    cr_assert_eq(strcmp(vec->data[2]->str, "'hi  '"), 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_OPERATOR);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[3]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_strange_word)
{
    int i = 0;
    char *input = "hjuzegfajh'      'sdgqsdgsd";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(strcmp(vec->data[0]->str, "hjuzegfajh'      'sdgqsdgsd"), 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_strange_word2)
{
    int i = 0;
    char *input = "hjuzegfajh'      'sdgqsdgsd';'''''';;;;'";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(
        strcmp(vec->data[0]->str, "hjuzegfajh'      'sdgqsdgsd';'''''';;;;'"),
        0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_no_quotes)
{
    int i = 0;
    char *input = "yo 'yo'";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 3);
    cr_assert_eq(strcmp(vec->data[0]->str, "yo"), 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(strcmp(vec->data[1]->str, "'yo'"), 0);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_fail)
{
    int i = 0;
    char *input = " '   hello'  'couille";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 3);
    cr_assert_eq(strcmp(vec->data[0]->str, "'   hello'"), 0);
    cr_assert_eq(strcmp(vec->data[1]->str, "'couille"), 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, dblquotes_fail)
{
    int i = 0;
    char *input = " \"   hello\"  \"couille";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 3);
    cr_assert_eq(strcmp(vec->data[0]->str, "\"   hello\""), 0);
    cr_assert_eq(strcmp(vec->data[1]->str, "\"couille"), 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, dblquotes_two_words)
{
    int i = 0;
    char *input = "  \"  hello   \"   \"hi  \"  ";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 3);
    cr_assert_eq(strcmp(vec->data[0]->str, "\"  hello   \""), 0);
    cr_assert_eq(strcmp(vec->data[1]->str, "\"hi  \""), 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[2]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, dblquotes_in_quotes)
{
    int i = 0;
    char *input = "  \"  hello   '   'hi  \"  ";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(strcmp(vec->data[0]->str, "\"  hello   '   'hi  \""), 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, quotes_in_dblquotes)
{
    int i = 0;
    char *input = "  '  hello   \"   \"hi  '  ";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(strcmp(vec->data[0]->str, "'  hello   \"   \"hi  '"), 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    cr_assert_eq(vec->data[1]->type, PRETOKEN_EOF);
    free_pretoken_list(vec);
}

Test(prelexify, backslash)
{
    int i = 0;
    char *input = "\\h\\e\\ l\\l\\o";
    struct pretoken_vector *vec = prelexify(input, &i);
    cr_assert_eq(vec->size, 2);
    cr_assert_eq(strcmp(vec->data[0]->str, "\\h\\e\\ l\\l\\o"), 0);
    cr_assert_eq(vec->data[0]->type, PRETOKEN_WORD);
    free_pretoken_list(vec);
}
