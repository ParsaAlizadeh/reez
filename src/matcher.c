#include "matcher.h"

#include <stdlib.h>
#include <string.h>
#include "re.h"

static int _match_helper(RE **regex, char *text);
static int _match_one(RE *re, char text);
static int _match_set(RE *re, char text);
static int _match_here(RE **regex, char *text);
static int _match_star(RE **regex, char *text, RE *re);
static int _match_maybe(RE **regex, char *text, RE *re);

int ismatch(vector *regex, char *text) {
    return _match_helper((RE **)regex->elems, text);
}

static int _match_helper(RE **regex, char *text) {
    if (regex[0] && regex[0]->control && regex[0]->chr == '^') {
        return _match_here(++regex, text);
    }
    do {
        if (_match_here(regex, text)) {
            return 1;
        }
    } while (*(text++) != '\0');
    return 0;
}

static int _match_one(RE *re, char text) {
    if (text == '\0') {
        return 0;
    }
    if (re->set) {
        return _match_set(re, text);
    }
    return (re->control && re->chr == '.') || re->chr == text;
}

static int _match_set(RE *re, char text) {
    char *set = re->set;
    while (*set != '\0' && *set != ']') {
        if (*(set++) == text) {
            return !re->exclude;
        }
    }
    return re->exclude;
}

static int _match_here(RE **regex, char *text) {
    if (!regex[0]) {
        return 1;
    }
    if (regex[0]->control && regex[0]->chr == '$') {
        return text[0] == '\0' || text[0] == '\n';
    }
    if (regex[0]->closure == RE_STAR || regex[0]->closure == RE_PLUS) {
        return _match_star(regex + 1, text, regex[0]);
    }
    if (regex[0]->closure == RE_MAYBE) {
        return _match_maybe(regex + 1, text, regex[0]);
    }
    if (_match_one(regex[0], text[0])) {
        return _match_here(++regex, ++text);
    }
    return 0;
}

static int _match_star(RE **regex, char *text, RE *re) {
    if (re->closure == RE_PLUS && !_match_one(re, *(text++))) {
        return 0;
    }
    do {
        if (_match_here(regex, text)) {
            return 1;
        }
    } while (_match_one(re, *(text++)));
    return 0;
}

static int _match_maybe(RE **regex, char *text, RE *re) {
    if (_match_here(regex, text)) {
        return 1;
    }
    return _match_one(re, text[0]) && _match_here(regex, text + 1);
}
