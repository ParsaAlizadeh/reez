#ifndef MATCHER_H
#define MATCHER_H

#include "vector.h"

/*
 * given a compile regex as a vector of REs, searches that pattern inside
 * text. '\n' and '\0' are both considered as the end of the text. vector
 * remains untouched.
 */
int ismatch(const vector *regex, const char *text);

#endif
