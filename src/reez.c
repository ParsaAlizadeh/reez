#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "matcher.h"
#include "re.h"

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
    FILE *file;
    if (!(file = fopen(argv[2], "r"))) {
        err(EXIT_FAILURE, "failed to open the file");
    }
    char *line = NULL;
    size_t n_line;
    int found = 0;
    while (getline(&line, &n_line, file) >= 0) {
        if (match(regex, line)) {
            found++;
            printf("%s", line);
        }
    }
    (void)fclose(file);
    if (line) {
        free(line);
    }
    vector_free_all(regex);
    if (!found) {
        fprintf(stderr, "found nothing\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
