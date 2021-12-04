#include <stdio.h>
#include <stdlib.h>

#include "hash_map.h"

static void print_pair_list(struct pair_list *list)
{
    int res = 0;
    if (list)
    {
        res = 1;
        printf("%s: %s", list->key, list->value);
        list = list->next;
    }
    while (list)
    {
        printf(", %s: %s", list->key, list->value);
        list = list->next;
    }
    if (res)
        printf("\n");
}

/**
 * @brief for debug: dump the hash map on stdout
 *
 * @param hash
 */
void hash_map_dump(struct hash_map *hash)
{
    if (hash == NULL || hash->data == NULL)
        return;
    for (size_t i = 0; i < hash->size; i++)
    {
        print_pair_list(hash->data[i]);
    }
}
