CC = gcc
CFLAGS = -Wall -Wextra -O2
CDEBUG = -fsanitize=address -g3 -pg -Og
PROGRAM = reez
HEADERS = src/eprintf.h src/re.h src/nfa.h
SOURCES = src/eprintf.c src/re.c src/nfa.c src/reez.c

all: ${PROGRAM}
.PHONY: all

debug: CFLAGS += ${CDEBUG}
debug: ${PROGRAM}
.PHONY: debug

${PROGRAM}: ${HEADERS} ${SOURCES}
	${CC} ${CFLAGS} ${SOURCES} -o ${PROGRAM}

clean:
	rm -rf ${PROGRAM}
.PHONY: clean

test: ${PROGRAM} test/comb
.PHONY: test

test/comb: test/comb.c
	${CC} ${CFLAGS} $< -o $@

clean-test:
	rm -rf test/comb test/*.in test/*.out test/*.err
.PHONY: clean-test
