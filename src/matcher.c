#include "matcher.h"

#include <stdlib.h>
#include <string.h>

static char *_text = NULL;
static char *_beg = NULL;

static char _peek(void) {
    return *_text;
}

static char _pop(void) {
    if (*_text == '\0')
        return '\0';
    return *(_text++);
}

static int _match_start(RE *re, char *text);
static int _match_chain(RE *re);
static int _match_control(RE *re);
static int _match_one(RE *re);
static int _match_more(RE *re, RE *cont);
static int _match_maybe(RE *re, RE *cont);

int ismatch(const RE *re, const char *text) {
    return _match_start((RE *)re, (char *)text);
}

static int _match_start(RE *re, char *text) {
    _beg = text;
    _text = text;
    char *tmp;
    do {
        tmp = _text;
        if (_match_chain(re))
            return 1;
        _text = tmp;
    } while (_pop() != '\0');
    return 0;
}

static int _match_chain(RE *re) {
    if (re == NULL)
        return 1;
    if (RE_iscontrol(re) && re->c != '.')
        return _match_control(re) && _match_chain(re->next);
    if (re->closure == RE_STAR || re->closure == RE_PLUS)
        return _match_more(re, re->next);
    if (re->closure == RE_MAYBE)
        return _match_maybe(re, re->next);
    return _match_one(re) && _match_chain(re->next);
}

static int _match_control(RE *re) {
    if (re->c == '^')
        return _text == _beg;
    else if (re->c == '$')
        return _peek() == '\0';
    return 0; /* undefined control */
}

static int _match_more(RE *re, RE *cont) {
    if (re->closure == RE_PLUS && !_match_one(re))
        return 0;
    char *tmp;
    do {
        tmp = _text;
        if (_match_chain(cont))
            return 1;
        _text = tmp;
    } while (_match_one(re));
    return 0;
}

static int _match_maybe(RE *re, RE *cont) {
    char *tmp = _text;
    if (_match_chain(cont))
        return 1;
    _text = tmp;
    return _match_one(re) && _match_chain(cont);
}

static int _match_one(RE *re) {
    if (re->type == RE_TGROUP)
        return _match_chain(re->group);
    char textc;
    if ((textc = _pop()) == '\0')
        return 0;
    if (RE_iscontrol(re) && re->c)
        return 1;
    if (re->type == RE_TCHAR)
        return re->c == textc;
    if (re->type == RE_TSET) {
        for (const char *set = re->set; *set != '\0' && *set != ']'; set++)
            if (*set == textc)
                return !RE_isexclude(re);
        return RE_isexclude(re);
    }
    return 0; /* undefined type */
}
