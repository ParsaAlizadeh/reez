#ifndef _MATCHER_H
#define _MATCHER_H

#include "vector.h"

/*
 * Given a vector of REs _regex_, searches them inside null-terminated
 * _text_. '\n' is considered as the end of the text. Won't change _regex_.
 * Returns 1 if found, 0 otherwise.
 */
int match(vector *regex, char *text);

#endif
