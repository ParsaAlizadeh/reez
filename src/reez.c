#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

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
            }
        }
    }
    (void)fclose(file);
    if (line) {
        free(line);
    }
    return 0;
}

static void print_usage_exit(char *prog) {
    fprintf(stderr, "Usage: %s [-qvc] <pattern> <filename> [<filename>...]\n", prog);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
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
            print_usage_exit(argv[0]);
        }
    }
    if (argc < optind + 2) {
        print_usage_exit(argv[0]);
    }
    vector *regex;
    if (!(regex = RE_compile(argv[optind++]))) {
        fprintf(stderr, "failed to compile the pattern\n");
        exit(EXIT_FAILURE);
    }
    int count = 0;
    for (; optind < argc; optind++) {
        FILE *file = fopen(argv[optind], "r");
        if (!file) {
            err(EXIT_FAILURE, "failed to open %s", argv[optind]);
        }
        search_file(file, regex, &count);
    }
    vector_free(regex);
    if (!f_quiet && f_count) {
        printf("%d\n", count);
    }
    if (count == 0) {
        fprintf(stderr, "found nothing\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
