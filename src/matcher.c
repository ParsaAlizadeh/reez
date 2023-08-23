#include "matcher.h"

#include <stdlib.h>
#include <string.h>
#include "re.h"

static int _match_helper(const RE *regex, const char *text);
static int _match_star(const RE *regex, const char *text, const RE *re);
static int _match_maybe(const RE *regex, const char *text, const RE *re);

int ismatch(const vector *regex, const char *text) {
    return _match_helper((RE *)regex->mem, text);
}

inline static int endoftext(int c) {
    return c == '\0' || c == '\n' || c == '\r';
}

static int _match_set(const RE *re, const char textchr) {
    for (const char *set = re->set; *set != '\0' && *set != ']'; set++) {
        if (*set == textchr)
            return !re->exclude;
    }
    return re->exclude;
}

inline static int _match_one(const RE *re, const char textchr) {
    if (endoftext(textchr))
        return 0;
    if (re->set)
        return _match_set(re, textchr);
    return (re->control && re->chr == '.') || re->chr == textchr;
}

static int _match_here(const RE *regex, const char *text) {
    if (!is_RE(&regex[0]))  /* check for null RE at the end of vector */
        return 1;
    if (regex[0].control && regex[0].chr == '$')
        return endoftext(text[0]);
    if (regex[0].closure == RE_STAR || regex[0].closure == RE_PLUS)
        return _match_star(regex + 1, text, &regex[0]);
    if (regex[0].closure == RE_MAYBE)
        return _match_maybe(regex + 1, text, &regex[0]);
    if (_match_one(&regex[0], text[0]))
        return _match_here(++regex, ++text);
    return 0;
}

static int _match_helper(const RE *regex, const char *text) {
    if (is_RE(&regex[0]) && regex[0].control && regex[0].chr == '^')
        return _match_here(++regex, text);
    do {
        if (_match_here(regex, text))
            return 1;
    } while (!endoftext(*(text++)));
    return 0;
}

static int _match_star(const RE *regex, const char *text, const RE *re) {
    if (re->closure == RE_PLUS && !_match_one(re, *(text++)))
        return 0;
    do {
        if (_match_here(regex, text))
            return 1;
    } while (_match_one(re, *(text++)));
    return 0;
}

static int _match_maybe(const RE *regex, const char *text, const RE *re) {
    if (_match_here(regex, text))
        return 1;
    return _match_one(re, text[0]) && _match_here(regex, ++text);
}
