#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "matcher.h"
#include "re.h"

static int search_file(char *filename, vector *regex, int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return -1;
    }
    char *line = NULL;
    size_t n_line = 0;
    while (getline(&line, &n_line, file) >= 0) {
        if (match(regex, line)) {
            (*count)++;
            printf("%s", line);
        }
    }
    (void)fclose(file);
    if (line) {
        free(line);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <pattern> <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    vector *regex;
    if (!(regex = RE_compile(argv[1]))) {
        fprintf(stderr, "failed to compile the pattern\n");
        exit(EXIT_FAILURE);
    }
    int count = 0;
    search_file(argv[2], regex, &count);
    vector_free_all(regex);
    if (count == 0) {
        fprintf(stderr, "found nothing\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
