#ifndef RE_H
#define RE_H

#include <stdlib.h>

#define RE_DIGITS "0123456789"
#define RE_CONTROLS ".^$"

/* types of closures */
enum REClosure {
    RE_ONCE,
    RE_MAYBE,
    RE_STAR,
    RE_PLUS,
};

/* flags */
enum {
    RE_CONTROL = 1 << 0,
    RE_EXCLUDE = 1 << 2
};

/*
 * represents a single element of a regular expressions.
 */
typedef struct RE RE;

struct RE {
    char c;
    int flags, closure;
    const char *set;
    RE *next;
};

/*
 * compiles a regular expression into a vector of REs. on error returns
 * NULL.
 */
extern int RE_compile(const char *regex, RE **ret);

extern void RE_free(RE *);
extern int RE_isexclude(const RE *);
extern int RE_iscontrol(const RE *);

#endif
