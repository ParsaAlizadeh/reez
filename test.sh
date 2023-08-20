#!/bin/bash

tests=(
    test-substr
    test-randsubstr
    test-prefix
    test-suffix
    test-closure
    test-charset
)

test-substr() {
    for f in test/*.in; do
        do-test $f ''
        do-test $f 'a'
        do-test $f 'z'
        do-test $f '\.\.'
    done
}

test-prefix() {
    for f in test/*.in; do
        do-test $f '^'
        do-test $f '^bba'
        do-test $f '^\^'
    done
}

test-suffix() {
    for f in test/*.in; do
        do-test $f '$'
        do-test $f '23$'
        do-test $f '\$\$$'
    done
}

test-randsubstr() {
    for f in test/*.in; do
        ./test/comb 4 'ab12. ^$' | grep -Pv '.\^|\$.' | awk 'rand() < 0.02' | \
            while read line; do
                do-test $f "$line"
            done
    done
}

test-closure() {
    for f in test/*.in; do
        do-test $f 'a*'
        do-test $f '^a*$'
        do-test $f '^b+a?b+$'
        do-test $f '1\++2'
        do-test $f '1\*\*2*'
    done
}

test-charset() {
    for f in test/*.in; do
        do-test $f '\d'
        do-test $f '^2\D+'
        do-test $f '^[ba]*$'
        do-test $f '^[^2]+$'
        do-test $f '^a.*[.]\d$'
        do-test $f '\d+\+\d+\'
        do-test $f '1\.2.a$'
        do-test $f '[*+.]+[^?]?'
    done
}

test_failed=0
batch_failed=0

do-test() {
    local -r testfile="$1"; shift
    ./reez "$@" "$testfile" 2>/dev/null >test/reez.out
    grep -aP "$@" "$testfile" 2>/dev/null >test/grep.out
    if ! cmp -s test/reez.out test/grep.out; then
        wecho-bad "do-test $testfile '$@': files differ"
        : $(( test_failed += 1 ))
    elif (( verbose > 0 )); then
        wecho-ok "do-test $testfile '$@'"
    fi
}

test-all() {
    rm -f test/*.in test/*.out
    touch test/empty.in
    echo > test/single-line.in
    echo -n ' ' > test/single-char.in
    ./test/comb 5 'abz0129' > test/comb-char.in
    ./test/comb 5 "$(echo -e 'a1.?*+^$\[] \t')" > test/comb-special.in
    # cp test/comb test/binary.in # null chars inside lines
    for batch in "${tests[@]}"; do
        test_failed=0
        "$batch"
        if (( test_failed > 0 )); then
            wecho-bad "$batch failed on $test_failed test(s)"
            wecho
            : $(( batch_failed += 1 ))
        elif (( verbose > 0 )); then
            wecho-ok "$batch passed"
            wecho
        fi
    done
    if (( batch_failed > 0 )); then
        wecho-bad "failed on $batch_failed batche(s)"
    else
        wecho-ok "passed"
    fi
    rm -f test/*.in test/*.out
}

C_GREEN='\e[1;32m'
C_RED='\e[1;31m'
C_RESET='\e[0m'

wecho-ok() {
    wecho -e "${C_GREEN}OK: ""$@""${C_RESET}"
}

wecho-bad() {
    wecho -e "${C_RED}BAD: ""$@""${C_RESET}"
}

wecho() {
    echo >&2 "$@"
}

usage() {
    wecho "Usage: $0 [-h] [-v]"
    exit 1
}

verbose=0
while (( $# > 0 )); do
    case "$1" in
        '-v')
            : $(( verbose += 1 ))
            shift
        ;;
        *)
            usage
        ;;
    esac
done

make test && test-all
