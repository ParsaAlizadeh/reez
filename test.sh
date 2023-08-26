#!/bin/bash

tests=(
    test-substr
    test-randsubstr
    test-prefix
    test-suffix
    test-closure
    test-charset
)
opts=('  ' -c -v -n)

test-substr() {
    do-test ''
    do-test 'a'
    do-test 'z'
    do-test '.\.'
}

test-prefix() {
    do-test '^'
    do-test '^bba'
    do-test '^\^'
}

test-suffix() {
    do-test '$'
    do-test '23$'
    do-test '\$\$$'
    do-test '^$'
    do-test '^.$'
}

test-randsubstr() {
    ./test/comb 4 'ab12^$' | grep -Pv '.\^|\$.' | awk 'rand() < 0.02' | \
        while read line; do
            do-test "$line"
        done
}

test-closure() {
    do-test 'a*'
    do-test '^a*$'
    do-test '^b+a?b+$'
    do-test '1\++2'
    do-test '1\*\*2*'
}

test-charset() {
    do-test '\d'
    do-test '^2\D+'
    do-test '^[ba]*$'
    do-test '^[^2]+$'
    do-test '^a.*[.]\d$'
    do-test '\d+\+\d+\'
    do-test '1\.2.a$'
    do-test '[*+.]+[^?]?'
}

test_failed=0
batch_failed=0

do-test() {
    for testfile in test/*.in; do
        for testopt in "${opts[@]}"; do
            ./reez "$testopt" "$@" "$testfile" 2>/dev/null >test/reez.out
            grep -aP "$testopt" "$@" "$testfile" 2>/dev/null >test/grep.out
            if ! cmp -s test/reez.out test/grep.out; then
                wecho-bad "do-test $testopt '$@' $testfile: files differ"
                : $(( test_failed += 1 ))
            elif (( verbose > 0 )); then
                wecho-ok "do-test $testopt '$@' $testfile"
            fi
        done
    done
}

test-all() {
    rm -f test/*.in test/*.out
    touch test/empty.in
    echo > test/single-line.in
    echo -n ' ' > test/single-char.in
    ./test/comb 4 'ab01' > test/comb-char.in
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
    rm -f test/*.out
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
