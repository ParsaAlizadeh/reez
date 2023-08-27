#ifndef RE_H
#define RE_H

#include <stdlib.h>

#define RE_DIGITS "0123456789"  /* \d */
#define RE_CONTROLS ".^$"

enum REClosure {
    RE_ONCE,
    RE_MAYBE,   /* c? */
    RE_STAR,    /* c* */
    RE_PLUS,    /* c+ */
};

enum REFlag {
    RE_CONTROL = 1 << 0,
    RE_EXCLUDE = 1 << 2
};

typedef struct RE RE;
struct RE {
    char c;
    int flags, closure;
    const char *set;        /* [0-9] */
    RE *next;
};

/*
 * check for a valid regular expression. if ret is not null, set *ret to
 * the resulting RE. on error returns -1.
 */
extern int RE_compile(const char *regex, RE **ret);

extern void RE_free(RE *);              /* accepts null */
extern int RE_isexclude(const RE *);    /* [^a-z] */
extern int RE_iscontrol(const RE *);

#endif
