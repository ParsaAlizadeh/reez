#ifndef MATCHER_H
#define MATCHER_H

#include "re.h"

/*
 * given a compile regex, searches that pattern inside text. regex remains
 * untouched.
 */
int ismatch(const RE *re, const char *text);

#endif
