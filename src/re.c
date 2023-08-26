#include "re.h"

#include <stdlib.h>
#include <string.h>
#include "eprintf.h"

static const RE _RE_DEFAULT = {
    .c = '\0',
    .flags = 0,
    .closure = RE_ONCE,
    .set = NULL,
    .next = NULL,
};

static RE *_RE_edup(const RE *s) {
    RE *re = emalloc(sizeof(RE));
    memcpy(re, s, sizeof(RE));
    return re;
}

static int _compile_escape(const char *regex, RE *re) {
    if (regex[1] == '\0')
        return -1;
    if (regex[1] == 't') {
        re->c = '\t';
        return 0;
    }
    if (regex[1] == 'd' || regex[1] == 'D') {
        re->set = RE_DIGITS;
        if (regex[1] == 'D')
            re->flags |= RE_EXCLUDE;
        return 0;
    }
    re->c = regex[1];
    return 0;
}

static int _compile_closure(const char *regex, RE *re) {
    switch (*regex) {
    case '?':
        re->closure = RE_MAYBE;
        return 0;
    case '*':
        re->closure = RE_STAR;
        return 0;
    case '+':
        re->closure = RE_PLUS;
        return 0;
    default:
        return -1;
    }
}

static int _compile_set(const char *regex, RE *re) {
    char *close = strchr(regex, ']');
    if (!close)
        return -1;
    if (regex[1] == '^') {
        regex += 2;
        re->flags |= RE_EXCLUDE;
    } else
        regex++;
    re->set = regex;
    return 0;
}

int RE_compile(const char *regex, RE **ret) {
    if (*regex == '\0') {
        *ret = NULL;
        return 0;
    }
    RE re = _RE_DEFAULT;
    if (*regex == '\\') {
        if (_compile_escape(regex, &re) == -1)
            return -1;
        regex += 2;
    } else if (*regex == '[') {
        if (_compile_set(regex, &re) == -1)
            return -1;
        regex = strchr(regex, ']') + 1;
    } else {
        re.c = *regex;
        if (strchr(RE_CONTROLS, *regex) != NULL)
            re.flags |= RE_CONTROL;
        regex++;
    }
    if (_compile_closure(regex, &re) != -1)
        regex++;
    if (RE_compile(regex, &re.next) == -1)
        return -1;
    if (ret != NULL)
        *ret = _RE_edup(&re);
    return 0;
}

void RE_free(RE *re) {
    RE *next;
    for (; re != NULL; re = next) {
        next = re->next;
        free(re);
    }
}

int RE_isexclude(const RE *re) {
    return (re->flags & RE_EXCLUDE) != 0;
}

int RE_iscontrol(const RE *re) {
    return (re->flags & RE_CONTROL) != 0;
}
