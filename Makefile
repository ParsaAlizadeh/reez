CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=address -g3 -O2 -pg
PROGRAM = reez
SOURCES = ${wildcard src/*.c src/**/*.c}
OBJECTS = ${patsubst src/%.c,build/%.o,${SOURCES}}
DEPENDS = ${patsubst %.o,%.d,${OBJECTS}}

all: ${PROGRAM}
.PHONY: all

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
	${CC} -O2 $< -o $@

test/sample.txt: test/make-sample Makefile
	./test/make-sample 5 "ab12.?*+^$$\[] " > $@

clean-test:
	rm -rf test/make-sample test/*.txt
.PHONY: clean-test
