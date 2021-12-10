#pragma once

#include <stdbool.h>
#include <stddef.h>

struct pair_list_function
{
    char *key;
    struct ast_node *value;
    struct pair_list_function *next;
};

struct hash_map_function
{
    struct pair_list_function **data;
    size_t size;
};

size_t hash_func(const char *str);

struct hash_map_function *hash_map_func_init(size_t size);
void hash_map_func_free(struct hash_map_function *hash_map,
                        void (*free_value)(struct ast_node *));

bool hash_map_func_insert(struct hash_map_function *hash_map, char *key,
                          void *value, void (*free_value)(struct ast_node *));
// void hash_map_dump(struct hash_map_function *hash);
void *hash_map_func_get(const struct hash_map_function *hash, char *key);
bool hash_map_func_remove(struct hash_map_function *hash, char *key);
