#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

/**
 * (simple agregation)
 */
struct vector
{
    // The number of elements in the vector
    size_t size;
    // The maximum number of elements in the vector
    size_t capacity;
    // The elements themselves
    void **data;
};

/*
** Initialize the vector with `n` capacity.
** Returns `NULL` if an error occured.
*/
struct vector *vector_init(size_t n);

/*
** Release the memory used by the vector.
** Does nothing if `v` is `NULL`.
*/
void vector_destroy(struct vector *v);

/*
** Resize the vector to `n` capacity.
** Returns `NULL` if an error occured.
*/
struct vector *vector_resize(struct vector *v, size_t n);

/*
** Append an element to the end of the vector. Expand the vector if needed.
** Returns `NULL` if an error occured.
*/
struct vector *vector_append(struct vector *v, void *elt);

/*
** Display the vector contents on `stdout`.
** Displays `\n` if `v` is `NULL`.
*/
void vector_print(const struct vector *v);

/*
** Remove all the elements of the vector, and resize it to `n` capacity.
** Returns `NULL` if an error occured.
*/
struct vector *vector_reset(struct vector *v, size_t n);

/*
** Insert `n` at the index `i`. Expand the vector if needed.
** Returns `NULL` if an error occured.
*/
struct vector *vector_insert(struct vector *v, size_t i, void *elt);

/*
** Remove the element at the index `i`.
** Returns `NULL` if an error occured.
*/
struct vector *vector_remove(struct vector *v, size_t i);

/**
 * @brief free the element without freeing the vector
 */
void vector_apply_on_elts(struct vector *v, void (*func)(void *));

#endif /* !VECTOR_H */
