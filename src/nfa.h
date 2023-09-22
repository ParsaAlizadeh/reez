#ifndef NFA_H
#define NFA_H

#include <stdlib.h>
#include "vector.h"
#include "re.h"

typedef struct Node Node;
typedef struct Edge Edge;
typedef struct NFA NFA;

struct Node {
    int id;
    vector *adj; /* int adj[] */
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
    vector *node;
    vector *edge;
    int *mark;
    int start, finish;
};

NFA *NFA_new(void);
void NFA_free(NFA *);
Node *NFA_new_node(NFA *);
Node *NFA_get_node(NFA *, int nodei);
Edge *NFA_new_edge(NFA *, int nodei, int nodej);
Edge *NFA_new_eps_edge(NFA *, int nodei, int nodej);
Edge *NFA_get_edge(NFA *, int edgei);
int NFA_build(NFA *, const RE *);
void NFA_traverse(NFA *, const char *text, int ntext);
int NFA_match(NFA *, const char *text, int ntext);

#endif
