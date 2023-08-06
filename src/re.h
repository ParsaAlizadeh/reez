#ifndef _RE_H
#define _RE_H

typedef struct _RE {
    char chr;
    int special, star;
} RE;

/*
 * Allocates and initialize a RE and return it. Returns NULL on error and
 * set errno
 */
RE *RE_new();

/*
 * Compile a regular expression into a null-terminated array of REs.
 * Returns NULL on error but *may not* set errno
 */
RE **RE_compile(char *regex);

#endif
