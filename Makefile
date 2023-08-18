CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=address -g3 -O2
PROGRAM = reez
SOURCES = ${wildcard src/*.c src/**/*.c}
OBJECTS = ${patsubst src/%.c,build/%.o,${SOURCES}}
DEPENDS = ${patsubst %.o,%.d,${OBJECTS}}

all: ${PROGRAM}

${PROGRAM}: ${OBJECTS}
	${CC} ${CFLAGS} ${OBJECTS} -o ${PROGRAM}

build/%.o: src/%.c
	@mkdir -p ${dir $@}
	${CC} ${CFLAGS} -c -MMD $< -o $@

-include ${DEPENDS}

clean:
	rm -rf build/ ${PROGRAM}
.PHONY: clean

test: ${PROGRAM} test/sample.txt
	./test/test.sh ./${PROGRAM}
.PHONY: test

test/make-sample: test/make-sample.c
	${CC} -O2 $< -o $@

test/sample.txt: test/make-sample
	./test/make-sample 5 "abc123 " > $@

clean-test:
	rm -rf test/make-sample test/sample.txt
.PHONY: clean-test
