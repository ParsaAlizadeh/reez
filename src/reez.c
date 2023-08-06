#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "matcher.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <pattern> <filename>\n", argv[0]);
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
        if (match(argv[1], line)) {
            found++;
            printf("%s", line);
        }
    }
    if (line)
        free(line);
    (void)fclose(file);
    if (!found) {
        fprintf(stderr, "found nothing\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
