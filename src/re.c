#include "re.h"

#include <stdlib.h>
#include "vector.h"

RE *RE_new() {
    RE *re = (RE *)malloc(sizeof(RE));
    if (!re)
        return NULL;
    re->special = 0;
    re->star = 0;
    return re;
}

static int RE_compile_helper(char *regex, vector *vec) {
    if (!*regex)
        return 0;
    RE *re;
    if (!(re = RE_new()))
        return -1;
    if (*regex == '\\') {
        switch (regex[1]) {
        case '\0':
            return -1;
        case 'n':
            re->chr = '\n';
            break;
        case 't':
            re->chr = '\t';
            break;
        default:
            re->chr = regex[1];
        }
        regex += 2;
        goto next;
    }
    re->chr = *regex;
    switch (*regex) {
    case '^':
    case '$':
    case '.':
        re->special = 1;
    }
    regex++;
    if (*regex == '*') {
        re->star = 1;
        regex++;
    }
next:
    vector_push(vec, re);
    return RE_compile_helper(regex, vec);
}

RE **RE_compile(char *regex) {
    vector *vec;
    if (!(vec = vector_new()))
        return NULL;
    if (RE_compile_helper(regex, vec) < 0) {
        vector_free_all(vec);
        return NULL;
    }
    return (RE **)vector_free(vec);
}
