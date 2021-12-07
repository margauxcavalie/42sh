#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <utils/alloc.h>

struct vector *vector_init(size_t n)
{
    // WILL NOT BE 0
    struct vector *new = xmalloc(sizeof(struct vector));
    if (!new)
        return NULL;
    new->size = 0;
    new->capacity = n;
    new->data = xmalloc(sizeof(void *) * n);
    if (!new->data)
    {
        free(new);
        return NULL;
    }
    return new;
}

void vector_destroy(struct vector *v)
{
    if (!v)
        return;
    if (v->data)
        free(v->data);
    free(v);
}

struct vector *vector_resize(struct vector *v, size_t n)
{
    // VECTOR NOT NULL and N > 0
    if (n == v->size)
        return v;
    void **tmp = xrealloc(v->data, sizeof(void *) * n);
    if (!tmp)
        return NULL;
    v->data = tmp;
    v->capacity = n;
    return v;
}

void vector_print(const struct vector *v)
{
    if (!v || v->size == 0)
    {
        return;
    }
    for (size_t i = 0; i < v->size - 1; i++)
    {
        printf("'%p',", v->data[i]);
    }
    printf("'%p'", v->data[v->size - 1]);
}

struct vector *vector_append(struct vector *v, void *elt)
{
    if (!v)
        return NULL;
    if (v->size + 1 >= v->capacity)
    {
        v = vector_resize(v, v->capacity * 2);
        if (!v)
            return NULL;
    }
    v->data[v->size] = elt;
    v->size += 1;
    return v;
}

struct vector *vector_reset(struct vector *v, size_t n)
{
    if (!v)
        return NULL;
    v = vector_resize(v, n);
    if (!v)
        return NULL;
    v->size = 0;
    v->capacity = n;
    return v;
}

struct vector *vector_insert(struct vector *v, size_t i, void *elt)
{
    if (!v || i > v->size)
        return NULL;
    if (i == v->size)
        return vector_append(v, elt);
    if (v->size >= v->capacity)
    {
        v = vector_resize(v, v->capacity * 2);
        ;
        if (!v)
            return NULL;
    }
    for (size_t x = v->size - 1; x >= i && x <= v->size - 1; x--)
    {
        v->data[x + 1] = v->data[x];
    }
    v->data[i] = elt;
    v->size += 1;
    return v;
}

struct vector *vector_remove(struct vector *v, size_t i)
{
    if (!v || i >= v->size)
        return NULL;
    for (size_t x = i + 1; x < v->size; x++)
    {
        v->data[x - 1] = v->data[x];
    }
    v->size -= 1;
    if (v->size >= 2 && v->size < v->capacity / 2)
    {
        v = vector_resize(v, v->capacity / 2);
        if (!v)
            return NULL;
    }
    return v;
}

void vector_apply_on_elts(struct vector *v, void (*func)(void *))
{
    for (size_t counter = 0; counter < v->size; counter++)
    {
        (*func)(v->data[counter]);
    }
}
