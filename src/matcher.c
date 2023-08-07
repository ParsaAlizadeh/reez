#include "matcher.h"

#include <stdlib.h>
#include "re.h"

static int match_helper(RE **regex, char *text);
static int match_one(RE *re, char text);
static int match_here(RE **regex, char *text);
static int match_star(RE **regex, char *text, RE *re);

int match(vector *regex, char *text) {
    return match_helper((RE **)regex->elems, text);
}

static int match_helper(RE **regex, char *text) {
    if (regex[0] && regex[0]->control && regex[0]->chr == '^') {
        return match_here(++regex, text);
    }
    do {
        if (match_here(regex, text)) {
            return 1;
        }
    } while (*(text++) != '\0');
    return 0;
}

static int match_one(RE *re, char text) {
    if (text == '\0') {
        return 0;
    }
    return (re->control && re->chr == '.') || re->chr == text;
}

static int match_here(RE **regex, char *text) {
    if (!regex[0]) {
        return 1;
    }
    if (regex[0]->control && regex[0]->chr == '$') {
        return !text[0];
    }
    if (regex[0]->closure == RE_STAR) {
        return match_star(regex + 1, text, regex[0]);
    }
    if (match_one(regex[0], text[0])) {
        return match_here(++regex, ++text);
    }
    return 0;
}

static int match_star(RE **regex, char *text, RE *re) {
    do {
        if (match_here(regex, text)) {
            return 1;
        }
    } while (match_one(re, *(text++)));
    return 0;
}
