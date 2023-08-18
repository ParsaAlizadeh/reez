#include <stdio.h>
#include <stdlib.h>

enum {
    N = 100
};

char buf[N];

void generate(int i, int length, const char *charset) {
    if (i == length) {
        printf("%s\n", buf);
        return;
    }
    for (const char *s = charset; *s != '\0'; s++) {
        buf[i] = *s;
        generate(i+1, length, charset);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s length charset\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int length = atoi(argv[1]);
    generate(0, length, argv[2]);
    exit(EXIT_SUCCESS);
}
