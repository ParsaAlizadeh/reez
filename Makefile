CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=address -g3 -O2
PROGRAM = reez
SOURCES = ${wildcard **/*.c}
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

test: ${PROGRAM}
	./test/test.sh ./${PROGRAM}
.PHONY: test
