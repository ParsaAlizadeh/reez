#ifndef NFA_H
#define NFA_H

#include <stdlib.h>
#include "re.h"

typedef struct Node Node;
typedef struct Edge Edge;
typedef struct NFA NFA;

#define array(type) struct {                    \
        int count, capacity;                    \
        type *items;                            \
    }

struct Node {
    int id;
    array(int) adj;
};

enum EdgeFlags {
    EDGE_CONTROL = 1 << 0,
    EDGE_EPSILON = 1 << 1,
    EDGE_SET = 1 << 2,
    EDGE_EXCLUDE = 1 << 3
};

struct Edge {
    char c;
    int id, flags, from, to;
    const char *set;
};

struct NFA {
    int start, finish;
    int *mark;
    array(Node) node;
    array(Edge) edge;
};

extern NFA *NFA_new(void);
extern void NFA_free(NFA *);
extern Node *NFA_new_node(NFA *);
extern Node *NFA_get_node(NFA *, int nodei);
extern Edge *NFA_new_edge(NFA *, int nodei, int nodej);
extern Edge *NFA_new_eps_edge(NFA *, int nodei, int nodej);
extern Edge *NFA_get_edge(NFA *, int edgei);
extern int NFA_build(NFA *, const RE *);
extern void NFA_traverse(NFA *, const char *text, int ntext);
extern int NFA_match(NFA *, const char *text, int ntext);

#endif
