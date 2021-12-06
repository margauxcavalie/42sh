#include <criterion/criterion.h>
#include <stdio.h>
#include <string.h>
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
/*
Test(build_key, dbl_dollar_quotes)
{
    char *var = "${$2}";
    int error = 0;
    size_t size = 0;
    char *key = build_key(var, &error, &size);
    cr_assert(strcmp(key, "$") == 0);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 4);
    free(key);
}*/

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
    cr_assert_eq(size, 3);
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
    struct hash_map *hash_map = var_hash_map_init();
    char *var = NULL;
    size_t size = 0;
    cr_assert_eq(expand_var(hash_map, var, &error, &size), NULL);
    cr_assert_eq(size, 0);
    hash_map_free(hash_map);
}

Test(var_expansion, no_dollar)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    char *var = malloc(sizeof(char) * 3);
    char *tmp = "ok";
    strcpy(var, tmp);
    size_t size = 0;
    cr_assert_eq(expand_var(hash_map, var, &error, &size), NULL);
    cr_assert_eq(size, 0);
    hash_map_free(hash_map);
    free(var);
}

Test(var_expansion, simple)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // setup the var ok manually var=ok
    char *key = malloc(sizeof(char) * 4);
    char *tmp = "var";
    char *value = malloc(sizeof(char) * 3);
    char *tmp2 = "ok";
    strcpy(key, tmp);
    strcpy(value, tmp2);
    bool updated = false;
    var_hash_map_insert(hash_map, key, value, &updated);
    // test code
    char *var = "$var";
    size_t size = 0;
    cr_assert(strcmp(expand_var(hash_map, var, &error, &size), "ok") == 0);
    cr_assert_eq(size, 4);
    cr_assert_eq(error, 0);
    hash_map_free(hash_map);
}

Test(var_expansion, dollar_alone)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // setup the var ok manually var=ok
    char *key = malloc(sizeof(char) * 4);
    char *tmp = "var";
    char *value = malloc(sizeof(char) * 3);
    char *tmp2 = "ok";
    strcpy(key, tmp);
    strcpy(value, tmp2);
    bool updated = false;
    var_hash_map_insert(hash_map, key, value, &updated);
    // test code
    char *var = "$";
    size_t size = 0;
    cr_assert_eq(expand_var(hash_map, var, &error, &size), NULL);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 1);
    hash_map_free(hash_map);
}

Test(var_expansion, simple_brackets)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // setup the var ok manually var=ok
    char *key = malloc(sizeof(char) * 4);
    char *tmp = "var";
    char *value = malloc(sizeof(char) * 3);
    char *tmp2 = "ok";
    strcpy(key, tmp);
    strcpy(value, tmp2);
    bool updated = false;
    var_hash_map_insert(hash_map, key, value, &updated);
    // test code
    char *var = "${var}";
    size_t size = 0;
    cr_assert(strcmp(expand_var(hash_map, var, &error, &size), "ok") == 0);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 6);
    hash_map_free(hash_map);
}

Test(var_expansion, simple_brackets_noise)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // setup the var ok manually var=ok
    char *key = malloc(sizeof(char) * 4);
    char *tmp = "var";
    char *value = malloc(sizeof(char) * 3);
    char *tmp2 = "ok";
    strcpy(key, tmp);
    strcpy(value, tmp2);
    bool updated = false;
    var_hash_map_insert(hash_map, key, value, &updated);
    // test code
    char *var = "${var}hello";
    size_t size = 0;
    cr_assert(strcmp(expand_var(hash_map, var, &error, &size), "ok") == 0);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 6);
    hash_map_free(hash_map);
}

Test(var_expansion, error)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // setup the var ok manually var=ok
    char *key = malloc(sizeof(char) * 4);
    char *tmp = "var";
    char *value = malloc(sizeof(char) * 3);
    char *tmp2 = "ok";
    strcpy(key, tmp);
    strcpy(value, tmp2);
    bool updated = false;
    var_hash_map_insert(hash_map, key, value, &updated);
    // test code
    char *var = "${var";
    size_t size = 0;
    cr_assert_eq(expand_var(hash_map, var, &error, &size), NULL);
    cr_assert_eq(error, 1);
    // cr_assert_eq(size, 0);
    hash_map_free(hash_map);
}

Test(var_expansion, simple_no_var)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // test code
    char *var = "$var";
    size_t size = 0;
    cr_assert(strcmp(expand_var(hash_map, var, &error, &size), "") == 0);
    cr_assert_eq(error, 0);
    cr_assert_eq(size, 4);
    hash_map_free(hash_map);
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
    cr_assert_eq(error, 0);
}

Test(check_all_brackets, simple_pass2)
{
    char *var =
        "${var} $ $$ yo dfs {dsfsd  wr echo for;; \n $ok   ${ok  drfdesfs ;}";
    int error = check_all_brackets(var);
    cr_assert_eq(error, 0);
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
    struct hash_map *hash_map = var_hash_map_init();
    char *var = "varhello";
    char *res = expand_all_string(hash_map, var, &error);
    cr_assert_eq(error, 0);
    cr_assert(strcmp(res, "varhello") == 0);
    free(res);
    hash_map_free(hash_map);
}

Test(expand_all_string, easy)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // setup the var ok manually var=ok
    char *key = malloc(sizeof(char) * 4);
    char *tmp = "var";
    char *value = malloc(sizeof(char) * 3);
    char *tmp2 = "ok";
    strcpy(key, tmp);
    strcpy(value, tmp2);
    bool updated = false;
    var_hash_map_insert(hash_map, key, value, &updated);
    // test code
    char *var = "${var}hello";
    char *res = expand_all_string(hash_map, var, &error);
    cr_assert_eq(error, 0);
    cr_assert(strcmp(res, "okhello") == 0);
    free(res);
    hash_map_free(hash_map);
}

Test(expand_all_string, easy2)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // setup the var ok manually var=ok
    char *key = malloc(sizeof(char) * 4);
    char *tmp = "var";
    char *value = malloc(sizeof(char) * 3);
    char *tmp2 = "ok";
    strcpy(key, tmp);
    strcpy(value, tmp2);
    bool updated = false;
    var_hash_map_insert(hash_map, key, value, &updated);
    // test code
    char *var = "hello$var";
    char *res = expand_all_string(hash_map, var, &error);
    cr_assert_eq(error, 0);
    cr_assert(strcmp(res, "hellook") == 0);
    free(res);
    hash_map_free(hash_map);
}

Test(expand_all_string, easy3)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // setup the var ok manually var=ok
    char *key = malloc(sizeof(char) * 4);
    char *tmp = "var";
    char *value = malloc(sizeof(char) * 3);
    char *tmp2 = "ok";
    strcpy(key, tmp);
    strcpy(value, tmp2);
    bool updated = false;
    var_hash_map_insert(hash_map, key, value, &updated);
    // test code
    char *var = "hello$var${var}$var";
    char *res = expand_all_string(hash_map, var, &error);
    cr_assert_eq(error, 0);
    cr_assert(strcmp(res, "hellookokok") == 0);
    free(res);
    hash_map_free(hash_map);
}

Test(expand_all_string, wrong_var)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // setup the var ok manually var=ok
    char *key = malloc(sizeof(char) * 4);
    char *tmp = "var";
    char *value = malloc(sizeof(char) * 3);
    char *tmp2 = "ok";
    strcpy(key, tmp);
    strcpy(value, tmp2);
    bool updated = false;
    var_hash_map_insert(hash_map, key, value, &updated);
    // test code
    char *var = "hello$var2${var2}$var";
    char *res = expand_all_string(hash_map, var, &error);
    cr_assert_eq(error, 0);
    cr_assert(strcmp(res, "hellook") == 0);
    free(res);
    hash_map_free(hash_map);
}

Test(expand_all_string, two_var)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // setup the var ok manually var=ok
    char *key = malloc(sizeof(char) * 4);
    char *tmp = "var";
    char *value = malloc(sizeof(char) * 3);
    char *tmp2 = "ok";
    strcpy(key, tmp);
    strcpy(value, tmp2);
    bool updated = false;
    var_hash_map_insert(hash_map, key, value, &updated);
    char *key2 = malloc(sizeof(char) * 10);
    char *tmp21 = "variable1";
    char *value2 = malloc(sizeof(char) * 7);
    char *tmp22 = ";ok;ok";
    strcpy(key2, tmp21);
    strcpy(value2, tmp22);
    var_hash_map_insert(hash_map, key2, value2, &updated);
    // test code
    char *var = "hello$variable1${var2}$var";
    char *res = expand_all_string(hash_map, var, &error);
    cr_assert_eq(error, 0);
    cr_assert(strcmp(res, "hello;ok;okok") == 0);
    free(res);
    hash_map_free(hash_map);
}

Test(expand_all_string, var_overwrite)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // setup the var ok manually var=ok
    char *key = malloc(sizeof(char) * 4);
    char *tmp = "var";
    char *value = malloc(sizeof(char) * 3);
    char *tmp2 = "ok";
    strcpy(key, tmp);
    strcpy(value, tmp2);
    bool updated = false;
    var_hash_map_insert(hash_map, key, value, &updated);
    char *key2 = malloc(sizeof(char) * 4);
    char *tmp21 = "var";
    char *value2 = malloc(sizeof(char) * 6);
    char *tmp22 = "pasOK";
    strcpy(key2, tmp21);
    strcpy(value2, tmp22);
    var_hash_map_insert(hash_map, key2, value2, &updated);
    // test code
    char *var = "hello$variable1${var2}$var";
    char *res = expand_all_string(hash_map, var, &error);
    cr_assert_eq(error, 0);
    cr_assert(strcmp(res, "hellopasOK") == 0);
    free(res);
    hash_map_free(hash_map);
}

Test(expand_all_string, var_not_found)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // setup the var ok manually var=ok
    char *key = malloc(sizeof(char) * 4);
    char *tmp = "var";
    char *value = malloc(sizeof(char) * 3);
    char *tmp2 = "ok";
    strcpy(key, tmp);
    strcpy(value, tmp2);
    bool updated = false;
    var_hash_map_insert(hash_map, key, value, &updated);
    // test code
    char *var = "hello$variable1${var2}";
    char *res = expand_all_string(hash_map, var, &error);
    cr_assert_eq(error, 0);
    cr_assert(strcmp(res, "hello") == 0);
    free(res);
    hash_map_free(hash_map);
}

Test(expand_all_string, back_slash)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // setup the var ok manually var=ok
    char *key = malloc(sizeof(char) * 4);
    char *tmp = "var";
    char *value = malloc(sizeof(char) * 3);
    char *tmp2 = "ok";
    strcpy(key, tmp);
    strcpy(value, tmp2);
    char *key2 = malloc(sizeof(char) * 2);
    char *tmp21 = "$";
    char *value2 = malloc(sizeof(char) * 15);
    char *tmp22 = "JesuisunDollar";
    strcpy(key2, tmp21);
    strcpy(value2, tmp22);
    bool updated = false;
    var_hash_map_insert(hash_map, key, value, &updated);
    var_hash_map_insert(hash_map, key2, value2, &updated);
    // test code
    char *var = "\\$";
    char *expected = "$";
    char *res = expand_all_string(hash_map, var, &error);
    cr_assert_eq(error, 0);
    cr_assert(strcmp(res, expected) == 0);
    free(res);
    hash_map_free(hash_map);
}

Test(expand_all_string, dollar_alone)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // setup the var ok manually var=ok
    char *key = malloc(sizeof(char) * 4);
    char *tmp = "var";
    char *value = malloc(sizeof(char) * 3);
    char *tmp2 = "ok";
    strcpy(key, tmp);
    strcpy(value, tmp2);
    char *key2 = malloc(sizeof(char) * 2);
    char *tmp21 = "$";
    char *value2 = malloc(sizeof(char) * 15);
    char *tmp22 = "JesuisunDollar";
    strcpy(key2, tmp21);
    strcpy(value2, tmp22);
    bool updated = false;
    var_hash_map_insert(hash_map, key, value, &updated);
    var_hash_map_insert(hash_map, key2, value2, &updated);
    // test code
    char *var = "    hello $ pd";
    char *expected = "    hello $ pd";
    char *res = expand_all_string(hash_map, var, &error);
    cr_assert_eq(error, 0);
    cr_assert(strcmp(res, expected) == 0);
    free(res);
    hash_map_free(hash_map);
}

Test(expand_all_string, back_slash2)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // setup the var ok manually var=ok
    char *key = malloc(sizeof(char) * 4);
    char *tmp = "var";
    char *value = malloc(sizeof(char) * 3);
    char *tmp2 = "ok";
    strcpy(key, tmp);
    strcpy(value, tmp2);
    char *key2 = malloc(sizeof(char) * 2);
    char *tmp21 = "$";
    char *value2 = malloc(sizeof(char) * 15);
    char *tmp22 = "JesuisunDollar";
    strcpy(key2, tmp21);
    strcpy(value2, tmp22);
    bool updated = false;
    var_hash_map_insert(hash_map, key, value, &updated);
    var_hash_map_insert(hash_map, key2, value2, &updated);
    // test code
    char *var = "\\\\$";
    char *expected = "\\$";
    char *res = expand_all_string(hash_map, var, &error);
    cr_assert_eq(error, 0);
    cr_assert(strcmp(res, expected) == 0);
    free(res);
    hash_map_free(hash_map);
}

Test(expand_all_string, random_example)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // setup the var ok manually var=ok
    char *key = malloc(sizeof(char) * 4);
    char *tmp = "var";
    char *value = malloc(sizeof(char) * 3);
    char *tmp2 = "ok";
    strcpy(key, tmp);
    strcpy(value, tmp2);
    char *key2 = malloc(sizeof(char) * 2);
    char *tmp21 = "$";
    char *value2 = malloc(sizeof(char) * 15);
    char *tmp22 = "JesuisunDollar";
    strcpy(key2, tmp21);
    strcpy(value2, tmp22);
    bool updated = false;
    var_hash_map_insert(hash_map, key, value, &updated);
    var_hash_map_insert(hash_map, key2, value2, &updated);
    // test code
    char *var = "char *var = ${var} $ $$ yo dfs {dsfsd  wr echo for;; \n $ok   "
                "\\${ok  drfdesfs ;a";
    char *expected =
        "char *var = ok $ JesuisunDollar yo dfs {dsfsd  wr echo for;; \n    "
        "${ok  drfdesfs ;a";
    char *res = expand_all_string(hash_map, var, &error);
    cr_assert_eq(error, 0);
    cr_assert(strcmp(res, expected) == 0);
    free(res);
    hash_map_free(hash_map);
}

Test(expand_all_string, random_example2)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // setup the var ok manually var=ok
    char *key = malloc(sizeof(char) * 4);
    char *tmp = "var";
    char *value = malloc(sizeof(char) * 3);
    char *tmp2 = "ok";
    strcpy(key, tmp);
    strcpy(value, tmp2);
    char *key2 = malloc(sizeof(char) * 2);
    char *tmp21 = "$";
    char *value2 = malloc(sizeof(char) * 15);
    char *tmp22 = "JesuisunDollar";
    strcpy(key2, tmp21);
    strcpy(value2, tmp22);
    bool updated = false;
    var_hash_map_insert(hash_map, key, value, &updated);
    var_hash_map_insert(hash_map, key2, value2, &updated);
    // test code
    char *var = "char *var = ${var} $ $$ yo dfs {dsfsd  wr echo for;; \n $ok   "
                "\\${ok $; $23 drfdesfs ;a";
    char *expected =
        "char *var = ok $ JesuisunDollar yo dfs {dsfsd  wr echo for;; \n    "
        "${ok $; 3 drfdesfs ;a";
    char *res = expand_all_string(hash_map, var, &error);
    cr_assert_eq(error, 0);
    cr_assert(strcmp(res, expected) == 0);
    free(res);
    hash_map_free(hash_map);
}
