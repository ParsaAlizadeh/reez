CC = gcc
CFLAGS = -Wall -Wextra -O2
CDEBUG = -fsanitize=address -g3 -pg -Og
PROGRAM = reez
SOURCES = ${wildcard src/*.c src/**/*.c}
OBJECTS = ${patsubst src/%.c,build/%.o,${SOURCES}}
DEPENDS = ${patsubst %.o,%.d,${OBJECTS}}

all: ${PROGRAM}
.PHONY: all

debug: CFLAGS += ${CDEBUG}
debug: ${PROGRAM}
.PHONY: debug

${PROGRAM}: ${OBJECTS}
	${CC} ${CFLAGS} ${OBJECTS} -o ${PROGRAM}

build/%.o: src/%.c
	@mkdir -p ${dir $@}
	${CC} ${CFLAGS} -c -MMD $< -o $@

-include ${DEPENDS}

clean:
	rm -rf build/ ${PROGRAM}
.PHONY: clean

test: ${PROGRAM} test/comb
.PHONY: test

test/comb: test/comb.c
	${CC} ${CFLAGS} $< -o $@

clean-test:
	rm -rf test/comb test/*.in test/*.out test/*.err
.PHONY: clean-test
