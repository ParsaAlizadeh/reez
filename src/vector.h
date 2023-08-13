#ifndef VECTOR_H
#define VECTOR_H

#include <sys/types.h>

typedef struct vector {
    void *mem;      /* void mem[.nbyte * .cap] */
    size_t nbyte;
    size_t nelem;
    size_t cap;
} vector;

/*
 * on error returns NULL and set errno
 */
vector *vector_new(size_t nbyte);

/*
 * on error returns -1 and set errno
 */
int vector_incrcap(vector *vec, size_t newcap);

/*
 * given invalid index returns NULL
 */
void *vector_at(vector *vec, size_t index);

/*
 * copies nbyte from src to the end of vector. returns pointer to that
 * location (e.g. vector_at(vec, nelem)). on error returns NULL and set
 * errno */
void *vector_push(vector *vec, const void *src);

/*
 * pops last element of the vector. if dest != NULL, copies nbyte that was
 * last element to dest. given empty vector returns -1.
 */
int vector_pop(vector *vec, void *dest);

void vector_free(vector *vec);

#endif
