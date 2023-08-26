#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "eprintf.h"
#include "matcher.h"

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

static int search_file(FILE *file, const RE *re, const char *filename, int opts) {
    int nmatch = 0;
    char *buf = NULL;
    size_t nbuf = 0;
    for (int lineno = 1; getline(&buf, &nbuf, file) >= 0; lineno++) {
        int n = strlen(buf);
        if (buf[n-1] == '\n')
            buf[n-1] = '\0';
        if (isinvert(opts) ^ ismatch(re, buf)) {
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

static void eprint_usage(char *prog) {
    eprintf("Usage: %s [-qvcn] <pattern> <filename> [<filename>...]", prog);
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
            eprint_usage(argv[0]);
        }
    }
    if (argc < optind + 2)
        eprint_usage(argv[0]);
    RE *re;
    if (RE_compile(argv[optind++], &re) == -1)
        eprintf("failed to compile the pattern");
    int nfile = argc - optind;
    int nmatch = 0;
    for (; optind < argc; optind++) {
        FILE *file = fopen(argv[optind], "r");
        if (file == NULL) {
            weprintf("can not open \"%s\":", argv[optind]);
            continue;
        }
        nmatch += search_file(file, re, nfile > 1 ? argv[optind] : NULL, optmask);
        (void)fclose(file);
    }
    RE_free(re);
    if (optmask & OPT_COUNT)
        printf("%d\n", nmatch);
    return nmatch == 0;
}
