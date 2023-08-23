#include "re.h"

#include <stdlib.h>
#include <string.h>

static const RE _RE_DEFAULT = {
    .chr = '\0',
    .control = 0,
    .closure = RE_ONCE,
    .exclude = 0,
    .set = NULL
};

static int _compile_escape(const char *regex, RE *re) {
    if (regex[1] == '\0')
        return -1;
    if (regex[1] == 't') {
        re->chr = '\t';
        return 0;
    }
    if (regex[1] == 'd' || regex[1] == 'D') {
        re->set = RE_DIGITS;
        re->exclude = regex[1] == 'D';
        return 0;
    }
    re->chr = regex[1];
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
        re->exclude = 1;
    } else {
        regex++;
    }
    re->set = regex;
    return 0;
}

static int _compile_helper(const char *regex, vector *vec) {
    if (!*regex) {
        if (vector_push(vec, &_RE_DEFAULT) == NULL)
            return -1;
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
        re.chr = *regex;
        if (strchr(RE_CONTROLS, *regex)) {
            re.control = 1;
        }
        regex++;
    }
    if (_compile_closure(regex, &re) != -1)
        regex++;
    if (vector_push(vec, &re) == NULL)
        return -1;
    return _compile_helper(regex, vec);
}

vector *RE_compile(const char *regex) {
    vector *vec = vector_new(sizeof(RE));
    if (vec == NULL)
        return NULL;
    if (_compile_helper(regex, vec) == -1) {
        vector_free(vec);
        return NULL;
    }
    return vec;
}
