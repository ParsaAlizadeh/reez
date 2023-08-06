#ifndef _VECTOR_H
#define _VECTOR_H

#include <sys/types.h>

typedef struct _vector {
    void **elems;
    size_t size, capacity;
} vector;

/*
 * Changes capacity of _vec_ to _new_cap_. Returns < 0 on allocation error
 * and set errno
 */
int vector_recap(vector *vec, size_t new_cap);

/*
 * Allocates a new vector and returns it. Initial capacity is 4. It will
 * not manage the allocation of elements. You need to free(3) the returned
 * vector and possibly all the elements you had pushed. Returns NULL on
 * allocation error and set errno.
 */
vector *vector_new();

/*
 * Pushes a new _elem_ at the end of _vec_. Changes _vec_->size and
 * probably _vec_->elems. Returns < 0 on error and set errno.
 */
int vector_push(vector *vec, void *elem);

#endif
