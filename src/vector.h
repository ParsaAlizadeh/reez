#ifndef _VECTOR_H
#define _VECTOR_H

#include <sys/types.h>

/*
 * null-terminated array of pointers with flexibility in size
 */
typedef struct vector {
    void **elems;
    size_t size;        // size of array
    size_t capacity;
} vector;

/*
 * allocates a new vector. on error returns NULL and set errno.
 */
vector *vector_new();

/*
 * push a new pointer to the end of vector. increase vec->size by 1. may
 * change vec->elems but the elements inside (e.g. vec->elems[i]) are
 * untouched. on error returns -1 and set errno.
 */
int vector_push(vector *vec, void *elem);

/*
 * frees the vector and returns vec->elems as a null-terminated array of
 * pointers.
 */
void **vector_free(vector *vec);

/*
 * frees the vector and all the pointers inside it
 */
void vector_free_all(vector *vec);

#endif
