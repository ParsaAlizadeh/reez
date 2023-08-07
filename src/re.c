#include "re.h"

#include <stdlib.h>

RE *RE_new() {
    RE *re = (RE *)malloc(sizeof(RE));
    if (!re) {
        return NULL;
    }
    re->chr = '\0';
    re->control = 0;
    re->closure = RE_ONCE;
    return re;
}

static int RE_compile_helper(char *regex, vector *vec) {
    if (!*regex) {
        return 0;
    }
    RE *re;
    if (!(re = RE_new())) {
        return -1;
    }
    if (*regex == '\\') {
        if (!regex[1]) {
            return -1;
        }
        re->chr = regex[1] == 't' ? '\t' : regex[1];
        regex += 2;
        goto next;
    }
    re->chr = *regex;
    if (*regex == '^' || *regex == '$' || *regex == '.') {
        re->control = 1;
    }
    regex++;
    if (*regex == '?') {
        re->closure = RE_MAYBE;
        regex++;
    } else if (*regex == '*') {
        re->closure = RE_STAR;
        regex++;
    } else if (*regex == '+') {
        re->closure = RE_PLUS;
        regex++;
    }
next:
    vector_push(vec, re);
    return RE_compile_helper(regex, vec);
}

vector *RE_compile(char *regex) {
    vector *vec;
    if (!(vec = vector_new())) {
        return NULL;
    }
    if (RE_compile_helper(regex, vec) < 0) {
        vector_free_all(vec);
        return NULL;
    }
    return vec;
}
