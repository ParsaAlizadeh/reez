#include "re.h"

#include <stdlib.h>
#include <string.h>
#include "eprintf.h"

static const RE _RE_DEFAULT = {
    .c = '\0',
    .type = RE_TCHAR,
    .flags = 0,
    .closure = RE_ONCE,
    .set = NULL,
    .next = NULL,
    .group = NULL
};

static RE *_RE_edup(const RE *s) {
    RE *re = emalloc(sizeof(RE));
    memcpy(re, s, sizeof(RE));
    return re;
}

static const char *_regex = NULL;

static char _peek(void) {
    return *_regex;
}

static char _pop(void) {
    if (_peek() == '\0')
        return '\0';
    return *(_regex++);
}

static void _unpop(char c) {
    if (c != '\0')
        _regex--;
}

static int _compile_escape(RE *re) {
    _pop(); /* '\' */
    char esc = _pop();
    if (esc == '\0')
        return -1;
    if (esc == 't') {
        re->c = '\t';
        return 0;
    }
    if (esc == 'd' || esc == 'D') {
        re->type = RE_TSET;
        re->set = RE_DIGITS;
        if (esc == 'D')
            re->flags |= RE_EXCLUDE;
        return 0;
    }
    re->c = esc;
    return 0;
}

static int _compile_closure(RE *re) {
    char clo = _pop();
    switch (clo) {
    case '?':
        re->closure = RE_MAYBE;
        break;
    case '*':
        re->closure = RE_STAR;
        break;
    case '+':
        re->closure = RE_PLUS;
        break;
    default:
        _unpop(clo);
        return -1;
    }
    return 0;
}

static int _compile_set(RE *re) {
    re->type = RE_TSET;
    char *close = strchr(_regex, ']');
    if (close == NULL)
        return -1;
    _pop(); /* '[' */
    if (_peek() == '^') {
        _pop();
        re->flags |= RE_EXCLUDE;
    }
    re->set = _regex;
    _regex = close + 1;
    return 0;
}

static int _compile_group(RE **ret) {
    RE *rep = NULL; /* final RE */
    RE **repp = &rep;
    while (_peek() != '\0' && _peek() != ')') {
        RE re = _RE_DEFAULT;
        switch (_peek()) {
        case '\\':
            if (_compile_escape(&re) == -1)
                goto fail;
            break;
        case '[':
            if (_compile_set(&re) == -1)
                goto fail;
            break;
        case '(':
            _pop(); /* '(' */
            re.type = RE_TGROUP;
            if (_compile_group(&re.group) == -1 || _peek() != ')')
                goto fail;
            _pop(); /* ')' */
            break;
        case '|':
            _pop(); /* '|' */
            re.type = RE_TBRANCH;
            re.next = rep;
            rep = _RE_edup(&re);
            repp = &rep->branch;
            continue;
            break;
        default:
            re.c = _pop();
            if (strchr(RE_CONTROLS, re.c) != NULL)
                re.type = RE_TCONTROL;
        }
        _compile_closure(&re);
        *repp = _RE_edup(&re);
        repp = &(*repp)->next;
    }
    if (ret == NULL)
        RE_free(rep);
    else
        *ret = rep;
    return 0;
 fail:
    RE_free(rep);
    return -1;
}

int RE_compile(const char *regex, RE **ret) {
    _regex = regex;
    return _compile_group(ret);
}

void RE_free(RE *re) {
    RE *next;
    for (; re != NULL; re = next) {
        next = re->next;
        RE_free(re->group);
        free(re);
    }
}

int RE_isexclude(const RE *re) {
    return (re->flags & RE_EXCLUDE) != 0;
}

int RE_iscontrol(const RE *re) {
    return re->type == RE_TCONTROL;
}
