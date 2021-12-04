#pragma once
#include <hash_map/hash_map.h>

char *expand_var(struct hash_map *hash_map, char *var, int *error);
bool var_hash_map_insert(struct hash_map *hash_map, char *key, char *value,
                         bool *updated);
struct hash_map *var_hash_map_init(void);
char *build_key(char *var, int *error); // only for tests
