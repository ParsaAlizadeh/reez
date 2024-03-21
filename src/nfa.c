#include "nfa.h"

#include <string.h>
#include "eprintf.h"

#define arrput(arr, x)                                                  \
    do {                                                                \
        if ((arr).count == (arr).capacity) {                            \
            if ((arr).capacity == 0)                                    \
                (arr).capacity = 8;                                     \
            else                                                        \
                (arr).capacity *= 2;                                    \
            (arr).items = erealloc((arr).items, sizeof((arr).items[0]) * (arr).capacity); \
        }                                                               \
        (arr).items[(arr).count++] = (x);                               \
    } while (0)

#define arrlast(arr) ((arr).items[(arr).count-1])

#define arrzero(arr)                            \
    do {                                        \
        (arr).count = (arr).capacity = 0;       \
        (arr).items = NULL;                     \
    } while (0)

#define arrfree(arr)                            \
    do {                                        \
        free((arr).items);                      \
        arrzero((arr));                         \
    } while (0)

void NFA_init(NFA *nfa) {
    arrzero(nfa->node);
    arrzero(nfa->edge);
    nfa->mark = NULL;
    nfa->start = nfa->finish = NFA_new_node(nfa)->id;
}

Node *NFA_new_node(NFA *nfa) {
    Node u = {0};
    u.id = nfa->node.count;
    arrput(nfa->node, u);
    return &arrlast(nfa->node);
}

Node *NFA_get_node(NFA *nfa, int i) {
    return &nfa->node.items[i];
}

Edge *NFA_get_edge(NFA *nfa, int i) {
    return &nfa->edge.items[i];
}

void NFA_clear(NFA *nfa) {
    for (int i = 0; i < nfa->node.count; i++) {
        arrfree(nfa->node.items[i].adj);
    }
    arrfree(nfa->node);
    arrfree(nfa->edge);
    free(nfa->mark);
}

Edge *NFA_new_edge(NFA *nfa, int i, int j) {
    Edge e = {0};
    e.id = nfa->edge.count;
    e.from = i;
    e.to = j;
    arrput(nfa->node.items[i].adj, e.id);
    arrput(nfa->edge, e);
    return &arrlast(nfa->edge);
}

Edge *NFA_new_eps_edge(NFA *nfa, int i, int j) {
    Edge *e = NFA_new_edge(nfa, i, j);
    e->flags |= EDGE_EPSILON;
    return e;
}

int NFA_build(NFA *nfa, const RE *re) {
    for (; re != NULL; re = re->next) {
        int start = nfa->finish;
        Node *newf;
        Edge *e;
        int f1, f2;
        switch (re->type) {
        case RE_TCONTROL:
        case RE_TCHAR:
            newf = NFA_new_node(nfa);
            e = NFA_new_edge(nfa, nfa->finish, newf->id);
            e->c = re->c;
            if (re->type == RE_TCONTROL)
                e->flags |= EDGE_CONTROL;
            nfa->finish = newf->id;
            break;
        case RE_TSET:
            newf = NFA_new_node(nfa);
            e = NFA_new_edge(nfa, nfa->finish, newf->id);
            e->set = re->set;
            e->flags |= EDGE_SET;
            if (re->flags & RE_EXCLUDE)
                e->flags |= EDGE_EXCLUDE;
            nfa->finish = newf->id;
            break;
        case RE_TGROUP:
            NFA_build(nfa, re->group);
            break;
        case RE_TBRANCH:
            f1 = NFA_build(nfa, re->next);
            nfa->finish = start;
            f2 = NFA_build(nfa, re->branch);
            NFA_new_eps_edge(nfa, f2, f1);
            nfa->finish = f1;
            goto done;
        }
        /* closure */
        if (start == nfa->finish || re->closure == RE_ONCE)
            ;
        else if (re->closure == RE_MAYBE)
            NFA_new_eps_edge(nfa, start, nfa->finish);
        else {
            newf = NFA_new_node(nfa);
            NFA_new_eps_edge(nfa, nfa->finish, newf->id);
            NFA_new_eps_edge(nfa, nfa->finish, start);
            if (re->closure == RE_STAR)
                NFA_new_eps_edge(nfa, start, nfa->finish);
            nfa->finish = newf->id;
        }
    }
 done:
    return nfa->finish;
}

static inline int *_NFA_get_mark(NFA *nfa, int u, size_t i) {
    int nnode = nfa->node.count;
    return &nfa->mark[i * nnode + u];
}

static inline int _Edge_accept(Edge *e, const char **textp, const char *beg) {
    if (e->flags & EDGE_EPSILON)
        return 1;
    if (e->flags & EDGE_CONTROL) {
        if (e->c == '^')
            return *textp == beg;
        else if (e->c == '$')
            return **textp == '\0';
        else if (e->c == '.')
            return *((*textp)++) != '\0';
        else
            eprintf("Edge_accept: undefined control %c", e->c);
    }
    if (e->flags & EDGE_SET) {
        char textc = *((*textp)++);
        if (textc == '\0')
            return 0;
        for (const char *set = e->set; *set != '\0' && *set != ']'; set++)
            if (*set == textc)
                return (e->flags & EDGE_EXCLUDE) == 0;
        return (e->flags & EDGE_EXCLUDE) != 0;
    }
    return *((*textp)++) == e->c;
}

static int _NFA_dfs(NFA *nfa, int uid, const char *text, const char *beg) {
    int *mark = _NFA_get_mark(nfa, uid, text - beg);
    if (*mark)
        return 0;
    *mark = 1;
    if (uid == nfa->finish)
        return 1;
    Node *u = NFA_get_node(nfa, uid);
    for (int i = 0; i < u->adj.count; i++) {
        int eid = u->adj.items[i];
        Edge *e = NFA_get_edge(nfa, eid);
        const char *now = text;
        if (_Edge_accept(e, &now, beg) && _NFA_dfs(nfa, e->to, now, beg))
            return 1;
    }
    if (uid == nfa->start && *text != '\0')
        return _NFA_dfs(nfa, uid, text+1, beg);
    return 0;
}

void NFA_traverse(NFA *nfa, const char *text, int ntext) {
    int nnode = nfa->node.count;
    int nmark = (ntext + 1) * nnode;
    nfa->mark = erealloc(nfa->mark, nmark * sizeof(int));
    memset(nfa->mark, 0, nmark * sizeof(int));
    _NFA_dfs(nfa, nfa->start, text, text);
}

int NFA_match(NFA *nfa, const char *text, int ntext) {
    NFA_traverse(nfa, text, ntext);
    for (int i = ntext; i >= 0; i--)
        if (*_NFA_get_mark(nfa, nfa->finish, i))
            return i;
    return -1;
}
