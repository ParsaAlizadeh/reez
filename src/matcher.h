#ifndef _MATCHER_H
#define _MATCHER_H

#include "vector.h"

/*
 * given a compile regex as a vector of REs, searches that pattern inside
 * text. '\n' and '\0' are both considered as the end of the text. vector
 * remains untouched.
 */
int ismatch(vector *regex, char *text);

#endif
