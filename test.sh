#!/bin/bash

tests=(
    test-substr
    test-prefix
    test-suffix
    test-closure
    test-charset
)

test-substr() {
    do-test ''
    do-test 'a'
    do-test 'z'
    do-test '12'
    do-test 'b  a '
    do-test '\.\.'
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
    local origsum expcsum
    origsum="$(./reez "$@" test/sample.txt 2>/dev/null | sha1sum)"
    expcsum="$(grep -P "$@" test/sample.txt 2>/dev/null | sha1sum)"
    if [[ "$origsum" != "$expcsum" ]]; then
        wecho-bad "do-test '""$@"\'
        : $(( test_failed += 1 ))
    elif (( verbose > 0 )); then
        wecho-ok "do-test '""$@"\'
    fi
}

test-all() {
    for batch in "${tests[@]}"; do
        test_failed=0
        "$batch"
        if (( test_failed > 0 )); then
            wecho-bad "$batch failed on $test_failed test(s)"
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
