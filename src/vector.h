#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

typedef struct vector {
    void *mem;      /* void mem[.nbyte * .cap] */
    size_t nbyte;
    size_t nelem;
    size_t cap;
} vector;

/*
 * dynamic array of vec->nelem elements, each vec->nbyte bytes
 */
vector *vector_new(size_t nbyte);

/*
 * returns element at a given index. checks for out of bound.
 */
void *vector_at(vector *vec, size_t index);

/*
 * copies nbyte from src to the new element at the end of vector. returns
 * pointer to that location (e.g. vector_at(vec, vec->nelem++)).
 */
void *vector_push(vector *vec, const void *src);

/*
 * pops last element of the vector. if dest != NULL, copies the last
 * element to dest. given empty vector returns -1.
 */
int vector_pop(vector *vec, void *dest);

/*
 * accepts null.
 */
void vector_free(vector *vec);

#endif
