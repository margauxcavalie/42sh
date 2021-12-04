#include <criterion/criterion.h>
#include <string.h>
#include <var_expansion/var_expansion.h>

Test(build_key, single_dollar)
{
    char *var = "$";
    int error = 0;
    char *key = build_key(var, &error);
    cr_assert_eq(key, NULL);
    cr_assert_eq(error, 0);
    free(key);
}

Test(build_key, dbl_dollar)
{
    char *var = "$$";
    int error = 0;
    char *key = build_key(var, &error);
    cr_assert(strcmp(key, "$") == 0);
    cr_assert_eq(error, 0);
    free(key);
}

Test(build_key, quadruple_dollar)
{
    char *var = "$$$$";
    int error = 0;
    char *key = build_key(var, &error);
    cr_assert(strcmp(key, "$") == 0);
    cr_assert_eq(error, 0);
    free(key);
}

Test(build_key, simple_key)
{
    char *var = "$key";
    int error = 0;
    char *key = build_key(var, &error);
    cr_assert(strcmp(key, "key") == 0);
    cr_assert_eq(error, 0);
    free(key);
}

Test(build_key, simple_key_brackets)
{
    char *var = "${key}";
    int error = 0;
    char *key = build_key(var, &error);
    cr_assert(strcmp(key, "key") == 0);
    cr_assert_eq(error, 0);
    free(key);
}

Test(build_key, simple_key_brackets_noise_behind)
{
    char *var = "${key}pouet";
    int error = 0;
    char *key = build_key(var, &error);
    cr_assert(strcmp(key, "key") == 0);
    cr_assert_eq(error, 0);
    free(key);
}

Test(build_key, simple_key_error)
{
    char *var = "${key";
    int error = 0;
    char *key = build_key(var, &error);
    cr_assert_eq(error, 1);
    cr_assert_eq(key, NULL);
}

Test(var_expansion, no_var)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    char *var = NULL;
    cr_assert_eq(expand_var(hash_map, var, &error), NULL);
    hash_map_free(hash_map);
}

Test(var_expansion, no_dollar)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    char *var = malloc(sizeof(char) * 3);
    char *tmp = "ok";
    strcpy(var, tmp);
    cr_assert_eq(expand_var(hash_map, var, &error), NULL);
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
    cr_assert(strcmp(expand_var(hash_map, var, &error), "ok") == 0);
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
    cr_assert_eq(expand_var(hash_map, var, &error), NULL);
    cr_assert_eq(error, 0);
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
    cr_assert(strcmp(expand_var(hash_map, var, &error), "ok") == 0);
    cr_assert_eq(error, 0);
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
    cr_assert(strcmp(expand_var(hash_map, var, &error), "ok") == 0);
    cr_assert_eq(error, 0);
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
    cr_assert_eq(expand_var(hash_map, var, &error), NULL);
    cr_assert_eq(error, 1);
    hash_map_free(hash_map);
}

Test(var_expansion, simple_no_var)
{
    int error = 0;
    struct hash_map *hash_map = var_hash_map_init();
    // test code
    char *var = "$var";
    cr_assert(strcmp(expand_var(hash_map, var, &error), "") == 0);
    cr_assert_eq(error, 0);
    hash_map_free(hash_map);
}
