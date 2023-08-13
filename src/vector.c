#include "vector.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

int vector_incrcap(vector *vec, size_t newcap) {
    void *newmem = realloc(vec->mem, newcap * vec->nbyte);
    if (newmem == NULL)
        return -1;
    vec->mem = newmem;
    vec->cap = newcap;
    return 0;
}

static void _vector_init(vector *vec, size_t nbyte) {
    vec->mem = NULL;
    vec->nelem = 0;
    vec->nbyte = nbyte;
    vec->cap = 0;
}

vector *vector_new(size_t nbyte) {
    vector *vec = malloc(sizeof(vector));
    if (vec == NULL)
        return NULL;
    _vector_init(vec, nbyte);
    return vec;
}

static void *_vector_unsafeat(vector *vec, size_t index) {
    return vec->mem + (index * vec->nbyte);
}

void *vector_at(vector *vec, size_t index) {
    if (index >= vec->nelem)
        return NULL;
    return _vector_unsafeat(vec, index);
}

void *vector_push(vector *vec, const void *src) {
    if (vec->nelem >= vec->cap) {
        size_t newcap = 2 * vec->nelem;
        if (vec->nelem == 0)
            newcap = 1;
        if (vector_incrcap(vec, newcap) == -1)
            return NULL;
    }
    void *dest = _vector_unsafeat(vec, vec->nelem++);
    memcpy(dest, src, vec->nbyte);
    return dest;
}

void vector_free(vector *vec) {
    free(vec->mem);
    free(vec);
}

int vector_pop(vector *vec, void *dest) {
    if (vec->nelem == 0)
        return -1;
    if (dest != NULL)
        memcpy(dest, _vector_unsafeat(vec, vec->nelem-1), vec->nbyte);
    vec->nelem--;
    return 0;
}
