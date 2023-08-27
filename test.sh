#!/bin/bash

tests=(
    test-substr
    test-randsubstr
    test-prefix
    test-suffix
    test-closure
    test-charset
)
opts=('' -c -v -n)

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
    ./test/comb 4 'a1. ^$' | grep -Pv '.\^|\$.' | awk 'rand() < 0.02' | \
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
    do-test 'a*a*a*a*$'
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
max_ms=0

do-test() {
    local cur_ms fmt logitem
    fmt="do-test %2s %-15s %-25s %3d ms"
    for testfile in test/*.in; do
        for testopt in "${opts[@]}"; do
            # SIGINT won't be catched by background jobs, so I can stop tests with single ^C
            grep --text --perl-regexp "$testopt" "$@" "$testfile" 2>/dev/null >test/grep.out
            /bin/time --quiet --format 'time: %U' -- ./reez "$testopt" "$@" "$testfile" >test/reez.out 2>test/reez.err
            cur_ms="$(awk '/^time/ { print 1000 * $2 }' test/reez.err)"
            max_ms="$(( cur_ms > max_ms ? cur_ms : max_ms ))"
            logitem=("$testopt" "'$*'" "$testfile" "$cur_ms")
            if ! cmp -s test/reez.out test/grep.out; then
                weprintf "$fmt    files differ" "${logitem[@]}"
                : $(( test_failed += 1 ))
            elif grep --quiet 'ERROR:' test/reez.err; then
                weprintf "$fmt    sanitizer error" "${logitem[@]}"
                : $(( test_failed += 1 ))
            elif (( verbose > 0 )); then
                wprintf "$fmt" "${logitem[@]}"
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
        echo $max_ms
        if (( test_failed > 0 )); then
            weprintf '%s failed on %d test(s)\n' "$batch" "$test_failed"
            : $(( batch_failed += 1 ))
        elif (( verbose > 0 )); then
            wprintf '%s passed\n' "$batch"
        fi
    done
    if (( batch_failed > 0 )); then
        weprintf 'failed on %d batch(es)' "$batch_failed"
    else
        wprintf 'passed all batches'
    fi
    wprintf 'maximum time %d ms' "$max_ms"
    rm -f test/*.out
}

C_GREEN='\e[1;32m'
C_RED='\e[1;31m'
C_RESET='\e[0m'

weprintf() {
    local -r fmt="$1"; shift
    wprintf "${C_RED}${fmt}${C_RESET}" "$@"
}

wprintf() {
    local -r fmt="$1"; shift
    printf >&2 "$fmt\n" "$@"
}

usage() {
    wprintf 'Usage: %s [-h] [-v]' "$0"
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
