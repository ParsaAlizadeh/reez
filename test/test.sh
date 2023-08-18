#!/bin/bash

tests=(
    test-substr
    test-prefix
    test-suffix
    test-closure
    test-set
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

test-set() {
    do-test '\d'
    do-test '^2\D+'
    do-test '^[ba]*$'
    do-test '^[^2]+$'
    do-test '^a.*[.]\d$'
    do-test '\d+\+\d+\'
    do-test '1\.2.a$'
    do-test '[*+.]+[^?]?'
}

failed=0
do-test() {
    local origsum expcsum
    origsum="$(./reez "$@" test/sample.txt 2>/dev/null | sha1sum)"
    expcsum="$(grep -P "$@" test/sample.txt 2>/dev/null | sha1sum)"
    if [[ "$origsum" == "$expcsum" ]]; then
        wecho-ok "do-test '""$@"\'
    else
        wecho-bad "do-test '""$@"\'
        : $(( failed += 1 ))
    fi
}

test-all() {
    for batch in "${tests[@]}"; do
        failed=0
        "$batch"
        if (( failed == 0 )); then
            wecho-ok "$batch passed"
        else
            wecho-bad "$batch failed on $failed test(s)"
        fi
        wecho
    done
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

test-all
