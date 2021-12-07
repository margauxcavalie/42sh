#include <criterion/criterion.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <var_expansion/var_expansion.h>

Test(build_key, single_dollar)
{
    char *var = "$ ";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    cr_assert_eq(key, NULL);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 1);
    free(key);
}

Test(build_key, dbl_dollar)
{
    char *var = "$$";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    cr_assert(strcmp(key, "$") == 0);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 2);
    free(key);
}

Test(build_key, dbl_dollar_tricky)
{
    char *var = "${$}";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    cr_assert(strcmp(key, "$") == 0);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 4);
    free(key);
}

Test(build_key, dbl_dollar_quotes)
{
    char *var = "${$2}";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    cr_assert_eq(key, NULL);
    cr_assert_eq(error, 1);
    free(key);
}

Test(build_key, dbl_dollar_quotes2)
{
    char *var = "${$non}";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    cr_assert_eq(key, NULL);
    cr_assert_eq(error, 1);
    free(key);
}

Test(build_key, bad_substitution)
{
    char *var = "${hello;k}";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    // printf("%s\n", key);
    cr_assert_eq(key, NULL);
    cr_assert_eq(error, 1);
    free(key);
}

Test(build_key, bad_substitution2)
{
    char *var = "${hel$lok}";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    // printf("%s\n", key);
    cr_assert_eq(key, NULL);
    cr_assert_eq(error, 1);
    free(key);
}

Test(build_key, bad_substitution3)
{
    char *var = "${2o}";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    // printf("%s\n", key);
    cr_assert_eq(key, NULL);
    cr_assert_eq(error, 1);
    free(key);
}

Test(build_key, quadruple_dollar)
{
    char *var = "$$$$";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    cr_assert(strcmp(key, "$") == 0);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 2);
    free(key);
}

Test(build_key, simple_key)
{
    char *var = "$key";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    cr_assert(strcmp(key, "key") == 0);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 4);
    free(key);
}

Test(build_key, simple_key_brackets)
{
    char *var = "${key}";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    cr_assert(strcmp(key, "key") == 0);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 6);
    free(key);
}

Test(build_key, simple_key_empty)
{
    char *var = "${}";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    cr_assert_eq(key, NULL);
    cr_assert_eq(error, 1);
    // cr_assert_eq(size, 0);
    free(key);
}

Test(build_key, simple_key_brackets_noise_behind)
{
    char *var = "${key}pouet";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    cr_assert(strcmp(key, "key") == 0);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 6);
    free(key);
}

Test(build_key, simple_key_brackets_noise)
{
    char *var = "$key{pouet}";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    cr_assert(strcmp(key, "key") == 0);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 4);
    free(key);
}

Test(build_key, simple_key_error)
{
    char *var = "${key";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    cr_assert_eq(error, 1);
    cr_assert_eq(key, NULL);
    cr_assert_eq(size, 0);
}

Test(build_key, simple_key_var_is_not_a_name)
{
    char *var = "$;";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    cr_assert_eq(NULL, key);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 1);
}

Test(build_key, simple_key_var_is_not_a_name2)
{
    char *var = "$;23";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    cr_assert_eq(NULL, key);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 1);
}

Test(build_key, simple_key_underscore)
{
    char *var = "$_23";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    cr_assert(strcmp(key, "_23") == 0);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 4);
    free(key);
}

Test(build_key, simple_key_begin_with_numb)
{
    char *var = "$2_23";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    // printf("%s\n", key);
    cr_assert(strcmp(key, "2") == 0);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 2);
    free(key);
}

Test(var_expansion, no_var)
{
    int error = 0;
    
    char *var = NULL;
    size_t size = 0;
    cr_assert_eq(expand_var( var, &error, &size), NULL);
    cr_assert_eq(size, 0);
    
}

Test(var_expansion, no_dollar)
{
    int error = 0;
    
    char *var = "ok";
    size_t size = 0;
    cr_assert(expand_var( var, &error, &size) == NULL);
    cr_assert_eq(size, 0);
    
}

Test(var_expansion, simple)
{
    int error = 0;
    
    // setup the var ok manually var=ok
    char *key = "var";
    char *value = "ok";
    int overwrite = 0;
    setenv( key, value, overwrite);
    // test code
    char *var = "$var";
    size_t size = 0;
    cr_assert(strcmp(expand_var( var, &error, &size), "ok") == 0);
    cr_assert_eq(size, 4);
    cr_assert_eq(error, 0);
    
}

Test(var_expansion, dollar_alone)
{
    int error = 0;
    
    // setup the var ok manually var=ok
    char *key = "var";
    char *value = "ok";
    int overwrite = 0;
    setenv( key, value, overwrite);
    // test code
    char *var = "$";
    size_t size = 0;
    cr_assert_eq(expand_var( var, &error, &size), NULL);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 1);
    
}

Test(var_expansion, simple_brackets)
{
    int error = 0;
    
    // setup the var ok manually var=ok
    char *key = "var";
    char *value = "ok";
    int overwrite = 0;
    setenv( key, value, overwrite);
    // test code
    char *var = "${var}";
    size_t size = 0;
    cr_assert(strcmp(expand_var( var, &error, &size), "ok") == 0);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 6);
    
}

Test(var_expansion, simple_brackets_noise)
{
    int error = 0;
    
    // setup the var ok manually var=ok
    char *key = "var";
    char *value = "ok";
    int overwrite = 0;
    setenv( key, value, overwrite);
    // test code
    char *var = "${var}hello";
    size_t size = 0;
    cr_assert(strcmp(expand_var( var, &error, &size), "ok") == 0);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 6);
    
}

Test(var_expansion, error)
{
    int error = 0;
    
    // setup the var ok manually var=ok
    char *key = "var";
    char *value = "ok";
    int overwrite = 0;
    setenv( key, value, overwrite);
    // test code
    char *var = "${var";
    size_t size = 0;
    cr_assert_eq(expand_var( var, &error, &size), NULL);
    cr_assert_eq(error, 1);
    // cr_assert_eq(size, 0);
    
}

Test(var_expansion, simple_no_var)
{
    int error = 0;
    
    // test code
    char *var = "$var";
    size_t size = 0;
    cr_assert(strcmp(expand_var( var, &error, &size), "") == 0);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 4);
    
}

Test(check_all_brackets, single_dollar)
{
    char *var = "$   ";
    int error = check_all_brackets(var);
    cr_assert_eq(error, 0);
}

Test(check_all_brackets, simple_error)
{
    char *var = "${";
    int error = check_all_brackets(var);
    cr_assert_eq(error, -1);
}

Test(check_all_brackets, simple_error2)
{
    char *var = "${}";
    int error = check_all_brackets(var);
    cr_assert_eq(error, -1);
}

Test(check_all_brackets, simple_error3)
{
    char *var =
        "${var}   yo  dfs dsfsd  wr echo for;; \n $ok   ${ok  drfdesfs ;";
    int error = check_all_brackets(var);
    cr_assert_eq(error, -1);
}

Test(check_all_brackets, simple_pass)
{
    char *var =
        "${var}   yo  dfs dsfsd  wr echo for;; \n $ok   ${ok  drfdesfs ;}";
    int error = check_all_brackets(var);
    cr_assert_eq(error, -1);
}

Test(check_all_brackets, simple_pass2)
{
    char *var =
        "${var} $ $$ yo dfs {dsfsd  wr echo for;; \n $ok   ${ok  drfdesfs ;}";
    int error = check_all_brackets(var);
    cr_assert_eq(error, -1);
}

Test(check_all_brackets, backslash_pass)
{
    char *var =
        "${var} $ $$ yo dfs {dsfsd  wr echo for;; \n $ok   \\${ok  drfdesfs ;";
    int error = check_all_brackets(var);
    cr_assert_eq(error, 0);
}

Test(expand_all_string, no_expansion)
{
    int error = 0;
    size_t size = 0;
    
    char *var = "\"varhello\"";
    char *res = expand_all_string( var, &error, &size);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 10);
    cr_assert(strcmp(res, "varhello") == 0);
    free(res);
    
}

Test(expand_all_string, easy)
{
    int error = 0;
    size_t size = 0;
    
    // setup the var ok manually var=ok
    char *key = "var";
    char *value = "ok";
    int overwrite = 0;
    setenv( key, value, overwrite);
    // test code
    char *var = "\"${var}hello\"";
    char *res = expand_all_string( var, &error, &size);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 13);
    cr_assert(strcmp(res, "okhello") == 0);
    free(res);
    
}

Test(expand_all_string, easy2)
{
    int error = 0;
    size_t size = 0;
    
    // setup the var ok manually var=ok
    char *key = "var";
    char *value = "ok";
    int overwrite = 0;
    setenv( key, value, overwrite);
    // test code
    char *var = "\"hello$var\"";
    char *res = expand_all_string( var, &error, &size);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 11);
    cr_assert(strcmp(res, "hellook") == 0);
    free(res);
    
}

Test(expand_all_string, easy3)
{
    int error = 0;
    size_t size = 0;
    
    // setup the var ok manually var=ok
    char *key = "var";
    char *value = "ok";
    int overwrite = 0;
    setenv( key, value, overwrite);
    // test code
    char *var = "\"hello$var${var}$var\"";
    char *res = expand_all_string( var, &error, &size);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 21);
    cr_assert(strcmp(res, "hellookokok") == 0);
    free(res);
    
}

Test(expand_all_string, wrong_var)
{
    int error = 0;
    size_t size = 0;
    
    // setup the var ok manually var=ok
    char *key = "var";
    char *value = "ok";
    int overwrite = 0;
    setenv( key, value, overwrite);
    // test code
    char *var = "\"hello$var2${var2}$var\"";
    char *res = expand_all_string( var, &error, &size);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 23);
    cr_assert(strcmp(res, "hellook") == 0);
    free(res);
    
}

Test(expand_all_string, two_var)
{
    int error = 0;
    size_t size = 0;
    
    // setup the var ok manually var=ok
    char *key = "var";
    char *value = "ok";
    int overwrite = 0;
    setenv( key, value, overwrite);
    char *key2 = "variable1";
    char *value2 = ";ok;ok";
    setenv( key2, value2, overwrite);
    // test code
    char *var = "\"hello$variable1${var2}$var\"";
    char *res = expand_all_string( var, &error, &size);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 28);
    cr_assert(strcmp(res, "hello;ok;okok") == 0);
    free(res);
    
}

Test(expand_all_string, var_overwrite)
{
    int error = 0;
    size_t size = 0;
    
    // setup the var ok manually var=ok
    char *key = "var";
    char *value = "ok";
    int overwrite = 1;
    setenv( key, value, overwrite);
    char *key2 = "var";
    char *value2 = "pasOK";
    setenv( key2, value2, overwrite);
    // test code
    char *var = "\"hello$variable1${var2}$var\"";
    char *res = expand_all_string( var, &error, &size);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 28);
    cr_assert(strcmp(res, "hellopasOK") == 0);
    free(res);
    
}

Test(expand_all_string, var_not_found)
{
    int error = 0;
    size_t size = 0;
    
    // setup the var ok manually var=ok
    char *key = "var";
    char *value = "ok";
    int overwrite = 0;
    setenv( key, value, overwrite);
    // test code
    char *var = "\"hello$variable1${var2}\"";
    char *res = expand_all_string( var, &error, &size);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 24);
    cr_assert(strcmp(res, "hello") == 0);
    free(res);
    
}

Test(expand_all_string, back_slash)
{
    int error = 0;
    size_t size = 0;
    
    // setup the var ok manually var=ok
    char *key = "var";
    char *value = "ok";
    char *key2 = "$";
    char *value2 = "JesuisunDollar";
    int overwrite = 0;
    setenv( key, value, overwrite);
    setenv( key2, value2, overwrite);
    // test code
    char *var = "\"\\$\"";
    char *expected = "$";
    char *res = expand_all_string( var, &error, &size);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 4);
    cr_assert(strcmp(res, expected) == 0);
    free(res);
    
}

Test(expand_all_string, dollar_alone)
{
    int error = 0;
    size_t size = 0;
    
    // setup the var ok manually var=ok
    char *key = "var";
    char *value = "ok";
    char *key2 = "$";
    char *value2 = "JesuisunDollar";
    int overwrite = 0;
    setenv( key, value, overwrite);
    setenv( key2, value2, overwrite);
    // test code
    char *var = "\"    hello $ pd\"";
    char *expected = "    hello $ pd";
    char *res = expand_all_string( var, &error, &size);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 16);
    cr_assert(strcmp(res, expected) == 0);
    free(res);
    
}

Test(expand_all_string, back_slash2)
{
    int error = 0;
    size_t size = 0;
    
    // setup the var ok manually var=ok
    char *key = "var";
    char *value = "ok";
    char *key2 = "$";
    char *value2 = "JesuisunDollar";
    int overwrite = 0;
    setenv( key, value, overwrite);
    setenv( key2, value2, overwrite);
    // test code
    char *var = "\"\\\\$\"";
    char *expected = "\\$";
    char *res = expand_all_string( var, &error, &size);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 5);
    cr_assert(strcmp(res, expected) == 0);
    free(res);
    
}

Test(expand_all_string, random_example)
{
    int error = 0;
    size_t size = 0;
    
    // setup the var ok manually var=ok
    char *key = "var";
    char *value ="ok";
    char *key2 = "$";
    char *value2 = "JesuisunDollar";
    int overwrite = 0;
    setenv( key, value, overwrite);
    setenv( key2, value2, overwrite);
    // test code
    char *var =
        "\"char *var = ${var} $' $$ yo dfs {dsfsd  wr echo for;; \n $ok   "
        "\\${ok  drfdesfs ;a\"";
    char *expected =
        "char *var = ok $' JesuisunDollar yo dfs {dsfsd  wr echo for;; \n    "
        "${ok  drfdesfs ;a";
    char *res = expand_all_string( var, &error, &size);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 82);
    cr_assert(strcmp(res, expected) == 0);
    free(res);
    
}

Test(expand_all_string, random_example2)
{
    int error = 0;
    size_t size = 0;
    
    // setup the var ok manually var=ok
    char *key = "var";
    char *value = "ok";
    char *key2 = "$";
    char *value2 = "JesuisunDollar";
    int overwrite = 0;
    setenv( key, value, overwrite);
    setenv( key2, value2, overwrite);
    // test code
    char *var =
        "\"char *var = ${var} $ $$ yo dfs {dsfsd  wr echo for;; \n $ok   "
        "\\${ok $; $23 drfdesfs ;a\"";
    char *expected =
        "char *var = ok $ JesuisunDollar yo dfs {dsfsd  wr echo for;; \n    "
        "${ok $; 3 drfdesfs ;a";
    char *res = expand_all_string( var, &error, &size);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 87);
    cr_assert(strcmp(res, expected) == 0);
    free(res);
    
}

Test(expand_all_string, forgot_quotes)
{
    int error = 0;
    size_t size = 0;
    
    char *var = "\"varhello";
    char *res = expand_all_string( var, &error, &size);
    cr_assert_eq(error, 1);
    cr_assert_eq(res, NULL);
    free(res);
    
}

Test(expand_all_string, forgot_quotes2)
{
    int error = 0;
    size_t size = 0;
    
    char *var = "varhello\"";
    char *res = expand_all_string( var, &error, &size);
    cr_assert_eq(error, 1);
    cr_assert_eq(res, NULL);
    free(res);
    
}

Test(expand_all_string, things_behind)
{
    int error = 0;
    size_t size = 0;
    
    char *var = "\"varhe\"llo";
    char *res = expand_all_string( var, &error, &size);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 7);
    cr_assert(strcmp(res, "varhe") == 0);
    free(res);
    
}

Test(expand_all_string, escape_gang)
{
    int error = 0;
    size_t size = 0;
    
    char *var = "\"\\\"  \\  \\' \\$ \\` \"";
    char *res = expand_all_string( var, &error, &size);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 18);
    cr_assert(strcmp(res, "\"  \\  \\' $ ` ") == 0);
    free(res);
    
}
