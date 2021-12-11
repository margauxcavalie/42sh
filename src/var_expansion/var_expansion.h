#pragma once
#include <hash_map/hash_map.h>

char *expand_var(struct hash_map *hash_map, char *var, int *error,
                 size_t *counter);
bool var_hash_map_insert(struct hash_map *hash_map, char *key, char *value);
struct hash_map *var_hash_map_init(void);
char *build_key(char *var, int *error, size_t *counter); // only for tests
int check_all_brackets(char *str);
char *expand_all_string(struct hash_map *hash_map, char *str, int *error,
                        size_t *counter);
void var_hash_map_free(struct hash_map *hash_map);
bool check_assignement_word(char *word);
bool var_hash_map_remove(struct hash_map *hash_map, char *key);
