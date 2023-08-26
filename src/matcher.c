#include "matcher.h"

#include <stdlib.h>
#include <string.h>

static int _match_helper(const RE *re, const char *text);
static int _match_star(const RE *re, const char *text, const RE *cur);
static int _match_maybe(const RE *re, const char *text, const RE *cur);

int ismatch(const RE *re, const char *text) {
    return _match_helper(re, text);
}

static int _match_set(const RE *re, const char textchr) {
    for (const char *set = re->set; *set != '\0' && *set != ']'; set++) {
        if (*set == textchr)
            return !RE_isexclude(re);
    }
    return RE_isexclude(re);
}

inline static int _match_one(const RE *re, const char textc) {
    if (textc == '\0')
        return 0;
    if (re->set)
        return _match_set(re, textc);
    return (RE_iscontrol(re) && re->c == '.') || re->c == textc;
}

static int _match_here(const RE *re, const char *text) {
    if (re == NULL)
        return 1;
    if (RE_iscontrol(re) && re->c == '$')
        return *text == '\0';
    if (re->closure == RE_STAR || re->closure == RE_PLUS)
        return _match_star(re->next, text, re);
    if (re->closure == RE_MAYBE)
        return _match_maybe(re->next, text, re);
    if (_match_one(re, *text))
        return _match_here(re->next, ++text);
    return 0;
}

static int _match_helper(const RE *re, const char *text) {
    if (re != NULL && RE_iscontrol(re) && re->c == '^')
        return _match_here(++re, text);
    do {
        if (_match_here(re, text))
            return 1;
    } while (*(text++) != '\0');
    return 0;
}

static int _match_star(const RE *re, const char *text, const RE *cur) {
    if (cur->closure == RE_PLUS && !_match_one(cur, *(text++)))
        return 0;
    do {
        if (_match_here(re, text))
            return 1;
    } while (_match_one(cur, *(text++)));
    return 0;
}

static int _match_maybe(const RE *re, const char *text, const RE *cur) {
    if (_match_here(re, text))
        return 1;
    return _match_one(cur, text[0]) && _match_here(re, ++text);
}
