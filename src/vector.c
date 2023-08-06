#include "vector.h"

#include <stdlib.h>
#include <errno.h>

struct _vector {
    void **elems;
    size_t size, capacity;
};

int vector_recap(vector *vec, size_t new_cap) {
    vec->elems = (void **)realloc(vec->elems, new_cap * sizeof(void *));
    if (!vec->elems)
        return -1;
    vec->capacity = new_cap;
    return 0;
}

vector *vector_new() {
    vector *vec = (vector *)malloc(sizeof(vector));
    if (!vec)
        return NULL;
    vec->elems = NULL;
    vec->size = 0;
    if (vector_recap(vec, 4) < 0) {
        free(vec);
        return NULL;
    }
    return vec;
}

int vector_push(vector *vec, void *elem) {
    if (vec->size + 1 == vec->capacity) {
        if (vector_recap(vec, 2 * vec->capacity) < 0)
            return -1;
    }
    vec->elems[vec->size++] = elem;
    vec->elems[vec->size] = NULL;
    return 0;
}
