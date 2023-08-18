#include <stdio.h>
#include <stdlib.h>

enum {
    N = 100
};

char buf[N];

void generate(int i, int maxlength, const char *charset) {
    buf[i] = '\0';
    printf("%s\n", buf);
    if (i == maxlength)
        return;
    const char *s;
    for (s = charset; *s != '\0'; s++) {
        buf[i] = *s;
        generate(i+1, maxlength, charset);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s maxlength charset\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int maxlength = atoi(argv[1]);
    generate(0, maxlength, argv[2]);
    exit(EXIT_SUCCESS);
}
