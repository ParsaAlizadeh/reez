#ifndef RE_H
#define RE_H

#include "vector.h"

#define RE_DIGITS "0123456789"
#define RE_CONTROLS ".^$"

typedef enum REClosure {
    RE_ONCE,
    RE_MAYBE,
    RE_STAR,
    RE_PLUS,
} REClosure;

/*
 * represents a single element of a regular expressions.
 */
typedef struct RE {
    char chr;
    int control, exclude;
    REClosure closure;
    const char *set;
} RE;

/*
 * compiles a regular expression into a vector of REs. on error returns
 * NULL.
 */
vector *RE_compile(const char *regex);

/*
 * some REs don't hold any information. like the last element of the vector
 */
int is_RE(const RE *re);

#endif
