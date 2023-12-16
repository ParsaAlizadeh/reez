#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "eprintf.h"
#include "nfa.h"

#define STB_DS_IMPLEMENTATION
#define STBDS_REALLOC(ctx,ptr,size) erealloc(ptr,size)
#define STBDS_FREE(ctx,ptr) free(ptr)
#include "stb_ds.h"

enum {
    OPT_INVERT = 1 << 0,
    OPT_COUNT = 1 << 1,
    OPT_QUIET = 1 << 2,
    OPT_LINENO = 1 << 3
};

static int isinvert(int opts) {
    return (opts & OPT_INVERT) != 0;
}

static int isquiet(int opts) {
    return (opts & OPT_QUIET) || (opts & OPT_COUNT);
}

static int search_file(FILE *file, NFA *nfa, const char *filename, int opts) {
    int nmatch = 0;
    char *buf = NULL;
    size_t nbuf = 0;
    for (int lineno = 1; getline(&buf, &nbuf, file) >= 0; lineno++) {
        int n = strlen(buf);
        if (buf[n-1] == '\n')
            buf[--n] = '\0';
        if (isinvert(opts) ^ (NFA_match(nfa, buf, n) != -1)) {
            nmatch++;
            if (!isquiet(opts)) {
                if (filename != NULL)
                    printf("%s:", filename);
                if (opts & OPT_LINENO)
                    printf("%d:", lineno);
                printf("%s\n", buf);
            }
        }
    }
    free(buf);
    return nmatch;
}

static void eprint_usage(void) {
    eprintf("Usage: %s [-qvcn] <pattern> [<filename>...]", getprogname());
}

int main(int argc, char *argv[]) {
    setprogname(argv[0]);
    int opt, optmask = 0;
    while ((opt = getopt(argc, argv, "qvcn")) != -1) {
        switch (opt) {
        case 'q':
            optmask |= OPT_QUIET;
            break;
        case 'v':
            optmask |= OPT_INVERT;
            break;
        case 'c':
            optmask |= OPT_COUNT;
            break;
        case 'n':
            optmask |= OPT_LINENO;
            break;
        default:
            eprint_usage();
        }
    }
    if (optind >= argc) // no pattern
        eprint_usage();
    RE *re;
    if (RE_compile(argv[optind++], &re) == -1)
        eprintf("failed to compile the pattern");
    NFA *nfa = NFA_new();
    NFA_build(nfa, re);
    int nfile = argc - optind;
    int nmatch = 0;
    if (optind < argc) {
        for (; optind < argc; optind++) {
            FILE *file = fopen(argv[optind], "r");
            if (file == NULL) {
                weprintf("can not open \"%s\":", argv[optind]);
                continue;
            }
            nmatch += search_file(file, nfa, nfile > 1 ? argv[optind] : NULL, optmask);
            (void)fclose(file);
        }
    } else {
        nmatch += search_file(stdin, nfa, NULL, optmask);
    }
    NFA_free(nfa);
    RE_free(re);
    if (optmask & OPT_COUNT)
        printf("%d\n", nmatch);
    return nmatch == 0;
}
