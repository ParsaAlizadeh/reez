#include "vector.h"

#include <string.h>
#include "eprintf.h"

static void _vector_recap(vector *vec, size_t newcap) {
    void *newmem = erealloc(vec->mem, newcap * vec->nbyte);
    vec->mem = newmem;
    vec->cap = newcap;
}

void vector_init(vector *vec, size_t nbyte) {
    vec->mem = NULL;
    vec->nelem = 0;
    vec->nbyte = nbyte;
    vec->cap = 0;
}

vector *vector_new(size_t nbyte) {
    vector *vec = emalloc(sizeof(vector));
    vector_init(vec, nbyte);
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
        _vector_recap(vec, newcap);
    }
    void *dest = _vector_unsafeat(vec, vec->nelem++);
    memcpy(dest, src, vec->nbyte);
    return dest;
}

int vector_pop(vector *vec, void *dest) {
    if (vec->nelem == 0)
        return -1;
    if (dest != NULL)
        memcpy(dest, _vector_unsafeat(vec, vec->nelem-1), vec->nbyte);
    vec->nelem--;
    return 0;
}

void vector_clear(vector *vec) {
    free(vec->mem);
    vector_init(vec, vec->nbyte);
}

void vector_free(vector *vec) {
    if (vec == NULL)
        return;
    free(vec->mem);
    free(vec);
}
