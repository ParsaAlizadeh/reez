#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "eprintf.h"
#include "matcher.h"
#include "re.h"

static int f_invert = 0, f_count = 0, f_quiet = 0;

static int search_file(FILE *file, vector *regex, int *count) {
    char *line = NULL;
    size_t n_line = 0;
    while (getline(&line, &n_line, file) >= 0) {
        if (f_invert ^ ismatch(regex, line)) {
            (*count)++;
            if (!f_quiet && !f_count) {
                printf("%s", line);
                if (line[strlen(line)-1] != '\n')
                    printf("\n");
            }
        }
    }
    (void)fclose(file);
    if (line) {
        free(line);
    }
    return 0;
}

static void eprint_usage(char *prog) {
    eprintf("Usage: %s [-qvc] <pattern> <filename> [<filename>...]", prog);
}

int main(int argc, char *argv[]) {
    setprogname(argv[0]);
    int opt;
    while ((opt = getopt(argc, argv, "qvc")) != -1) {
        switch (opt) {
        case 'q':
            f_quiet = 1;
            break;
        case 'v':
            f_invert = 1;
            break;
        case 'c':
            f_count = 1;
            break;
        default:
            eprint_usage(argv[0]);
        }
    }
    if (argc < optind + 2)
        eprint_usage(argv[0]);
    vector *regex;
    if ((regex = RE_compile(argv[optind++])) == NULL)
        eprintf("failed to compile the pattern");
    int count = 0;
    for (; optind < argc; optind++) {
        FILE *file = fopen(argv[optind], "r");
        if (file == NULL) {
            weprintf("can not open \"%s\":", argv[optind]);
            continue;
        }
        search_file(file, regex, &count);
    }
    vector_free(regex);
    if (!f_quiet && f_count)
        printf("%d\n", count);
    return count == 0;
}
