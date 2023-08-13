#include "vector.h"

#include <stdlib.h>
#include <errno.h>

static int vector_recap(vector *vec, size_t new_cap) {
    vec->elems = (void **)realloc(vec->elems, new_cap * sizeof(void *));
    if (!vec->elems) {
        return -1;
    }
    vec->capacity = new_cap;
    return 0;
}

vector *vector_new() {
    vector *vec = (vector *)malloc(sizeof(vector));
    if (!vec) {
        return NULL;
    }
    vec->elems = NULL;
    vec->size = 0;
    if (vector_recap(vec, 4) < 0) {
        free(vec);
        return NULL;
    }
    vec->elems[0] = NULL;
    return vec;
}

int vector_push(vector *vec, void *elem) {
    if (vec->size + 1 == vec->capacity) {
        if (vector_recap(vec, 2 * vec->capacity) < 0) {
            return -1;
        }
    }
    vec->elems[vec->size++] = elem;
    vec->elems[vec->size] = NULL;
    return 0;
}

void **vector_free(vector *vec) {
    void **elems = vec->elems;
    free(vec);
    return elems;
}

static void _free_all(void **elems) {
    while (*elems) {
        free(*(elems++));
    }
}

void vector_free_all(vector *vec) {
    void **elems = vector_free(vec);
    _free_all(elems);
    free(elems);
}
