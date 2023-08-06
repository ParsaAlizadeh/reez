#include "matcher.h"

int match(char *regex, char *text) {
    if (*regex == '^')
        return match_here(++regex, text);
    do {
        if (match_here(regex, text))
            return 1;
    } while (*(text++));
    return 0;
}

int match_here(char *regex, char *text) {
    if (!*regex)
        return 1;
    if (*regex == '$')
        return !*text;
    if (regex[1] == '*')
        return match_star(regex + 2, text, *regex);
    if (!*text)
        return 0;
    if (*regex == '.' || *regex == *text)
        return match_here(++regex, ++text);
    return 0;
}

int match_star(char *regex, char *text, char c) {
    do {
        if (match_here(regex, text))
            return 1;
    } while (c == '.' ? *(text++) : *(text++) == c);
    return 0;
}
