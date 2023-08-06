CC = gcc
CFLAGS = -Wall -Wextra -g -fsanitize=address -g3
PROGRAM = reez
SOURCES = ${wildcard **/*.c}
OBJECTS = ${patsubst src/%.c,build/%.o,${SOURCES}}
DEPENDS = ${patsubst %.o,%.d,${OBJECTS}}

all: ${PROGRAM}

${PROGRAM}: ${OBJECTS}
	${CC} ${CFLAGS} ${OBJECTS} -o ${PROGRAM}

build/%.o: src/%.c
	@mkdir -p ${dir $@}
	${CC} ${CFLAGS} -c -MMD  $< -o $@

-include ${DEPENDS}

clean:
	rm -rf build/ ${PROGRAM}
