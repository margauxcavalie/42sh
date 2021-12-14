#include "hash_map_function.h"

#include <parser/ast_node.h>
#include <parser/function/ast_function.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils/alloc.h>

// TODO
static struct pair_list_function *list_replace_key(struct pair_list_function *l,
                                                   char *key, void *value,
                                                   void (*free_value)(void *))
{
    while (l)
    {
        if (strcmp(l->key, key) == 0)
        {
            void *tmp = l->value;
            free_value(tmp);
            l->value = value;
            return l;
        }
        l = l->next;
    }
    return NULL;
}

static struct pair_list_function *pair_list_init(char *key, void *value)
{
    struct pair_list_function *list =
        xmalloc(sizeof(struct pair_list_function));
    if (!list)
        return NULL;
    list->key = strdup(key);
    list->value = value;
    list->next = NULL;
    return list;
}

/**
 * @brief init a new hash map
 *
 * @param size
 * @return struct hash_map_function*
 */
struct hash_map_function *hash_map_func_init(size_t size)
{
    struct hash_map_function *hash_map =
        xmalloc(sizeof(struct hash_map_function));
    if (!hash_map)
        return NULL;
    hash_map->data = zalloc(size * sizeof(struct pair_list_function));
    if (!hash_map->data)
        return NULL;
    hash_map->size = size;
    return hash_map;
}

/**
 * @brief sub function for hash_map free
 *
 * @param list
 */
static void free_pair_list(struct pair_list_function *list,
                           void (*free_value)(void *))
{
    while (list)
    {
        struct pair_list_function *tmp = list;
        list = list->next;
        free(tmp->key);
        free_value(tmp->value);
        free(tmp);
    }
}

/**
 * @brief free the hash map : complete agregation
 *
 * @param hash_map
 */
void hash_map_func_free(struct hash_map_function *hash_map,
                        void (*free_value)(void *))
{
    if (hash_map == NULL || hash_map->data == NULL)
        return;
    for (size_t i = 0; i < hash_map->size; i++)
    {
        free_pair_list(hash_map->data[i], free_value);
    }
    free(hash_map->data);
    free(hash_map);
}

/**
 * @brief insert a new pair list in the hash map
 *
 * @param list
 * @param key
 * @param value
 * @return struct pair_list_function*
 */
static struct pair_list_function *
pair_list_insert(struct pair_list_function *list, char *key, void *value,
                 void (*free_value)(void *))
{
    struct pair_list_function *elt = pair_list_init(key, value);
    if (!elt)
    {
        return NULL;
    }
    if (!list)
    {
        return elt;
    }
    else
    {
        struct pair_list_function *tmp =
            list_replace_key(list, key, value, free_value);
        if (tmp)
        {
            free(elt->key);
            free(elt);
            return list;
        }
        else
        {
            elt->next = list;
            return elt;
        }
    }
}

/**
 * @brief insert a new pair value / key in the hash map
 *
 * @param hash_map
 * @param key
 * @param value
 * @return true
 * @return false
 */
bool hash_map_func_insert(struct hash_map_function *hash_map, char *key,
                          void *value, void (*free_value)(void *))
{
    if (hash_map == NULL || hash_map->size == 0 || hash_map->data == NULL)
    {
        return false;
    }
    // printf("checkpoint :\n");
    size_t hash_value = hash_func(key);
    // printf("checkpoint :\n");
    // printf("hash value : %zu, hash size : %zu\n", hash_value,
    // hash_map->size); fflush(stdout);
    hash_value = hash_value % hash_map->size;
    // printf("checkpoint :\n");
    hash_map->data[hash_value] =
        pair_list_insert(hash_map->data[hash_value], key, value, free_value);
    // printf("checkpoint :\n");
    return true;
}

static void *get_pair_list(const struct pair_list_function *list, char *key)
{
    while (list)
    {
        if (strcmp(list->key, key) == 0)
            return list->value;
        list = list->next;
    }
    return NULL;
}

/**
 * @brief get the value associated to the key
 *
 * @param hash_map
 * @param key
 * @return const char* or NULL if key not found
 */
void *hash_map_func_get(const struct hash_map_function *hash_map, char *key)
{
    size_t hash_value = hash_func(key);
    if (hash_map == NULL || hash_map->size == 0 || hash_map->data == NULL)
    {
        return NULL;
    }
    hash_value = hash_value % hash_map->size;
    return get_pair_list(hash_map->data[hash_value], key);
}

static struct pair_list_function *list_find_pred(struct pair_list_function *l,
                                                 char *key)
{
    while (l->next)
    {
        if (strcmp(l->next->key, key) == 0)
            return l;
        l = l->next;
    }
    return NULL;
}

static struct pair_list_function *
remove_pair_list(struct pair_list_function *list, char *key, bool *res)
{
    struct pair_list_function *tmp;
    if (list == NULL)
    {
        *res = false;
        return NULL;
    }
    else
    {
        if (strcmp(list->key, key) == 0)
        {
            tmp = list->next;
            free(list);
            return tmp;
        }
        // printf("CHECKPOINT\n");
        struct pair_list_function *pred = list_find_pred(list, key);
        if (pred == NULL)
        {
            *res = false;
            return list;
        }
        tmp = pred->next;
        if (tmp)
        {
            pred->next = pred->next->next;
            free(tmp);
            return list;
        }
    }
    return list;
}

/**
 * @brief remove the key/value pair associated to the key
 *
 * @param hash_map
 * @param key
 * @return true
 * @return false
 */
bool hash_map_func_remove(struct hash_map_function *hash_map, char *key)
{
    size_t hash_value = hash_func(key);
    if (hash_map == NULL || hash_map->size == 0 || hash_map->data == NULL)
    {
        return false;
    }
    hash_value = hash_value % hash_map->size;
    bool res = true;
    hash_map->data[hash_value] =
        remove_pair_list(hash_map->data[hash_value], key, &res);
    free(key);
    return res;
}
