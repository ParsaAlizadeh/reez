#include "re.h"

#include <stdlib.h>
#include <string.h>

static RE *_new() {
    RE *re = (RE *)malloc(sizeof(RE));
    if (!re) {
        return NULL;
    }
    re->chr = '\0';
    re->control = 0;
    re->closure = RE_ONCE;
    re->exclude = 0;
    re->set = NULL;
    return re;
}

static int _compile_escape(char *regex, RE *re) {
    if (regex[1] == '\0') {
        return -1;
    }
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

static int _compile_closure(char *regex, RE *re) {
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

static int _compile_helper(char *regex, vector *vec) {
    if (!*regex) {
        return 0;
    }
    RE *re;
    if (!(re = _new())) {
        return -1;
    }
    if (*regex == '\\') {
        if (_compile_escape(regex, re) < 0) {
            free(re);
            return -1;
        }
        regex += 2;
    } else {
        re->chr = *regex;
        if (strchr(RE_CONTROLS, *regex)) {
            re->control = 1;
        }
        regex++;
    }
    if (_compile_closure(regex, re) >= 0) {
        regex++;
    }
    vector_push(vec, re);
    return _compile_helper(regex, vec);
}

vector *RE_compile(char *regex) {
    vector *vec;
    if (!(vec = vector_new())) {
        return NULL;
    }
    if (_compile_helper(regex, vec) < 0) {
        vector_free_all(vec);
        return NULL;
    }
    return vec;
}
