#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdbool.h>
#include <stddef.h>

struct pair_list
{
    char *key;
    void *value;
    struct pair_list *next;
};

struct hash_map
{
    struct pair_list **data;
    size_t size;
};

size_t hash(const char *str);

struct hash_map *hash_map_init(size_t size);
void hash_map_free(struct hash_map *hash_map, void (*free_value)(void *));

bool hash_map_insert(struct hash_map *hash_map, char *key, void *value,
                     void (*free_value)(void *));
// void hash_map_dump(struct hash_map *hash);
void *hash_map_get(const struct hash_map *hash, char *key);
bool hash_map_remove(struct hash_map *hash, char *key, void (*free_value)(void *));

#endif /* !HASH_MAP_H */
