#ifndef _RE_H
#define _RE_H

#include "vector.h"

typedef enum REClosure {
    RE_ONCE,
    RE_MAYBE,
    RE_STAR,
    RE_PLUS,
} REClosure;

typedef struct RE {
    char chr;
    int control;
    REClosure closure;
} RE;

/*
 * Compile a regular expression into a vector (null-terminated array) of
 * REs. Returns NULL on error but *may not* set errno
 */
vector *RE_compile(char *regex);

#endif
