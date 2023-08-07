#!/bin/bash

REEZ="$1"
SAMPLE='test/sample.txt'

PATTERNS=(
    '^a'
    '^b'
    '^a*$'
    '^b*a*$'
    '^b*a+$'
    '^b+a?b+$'
    '^b+a+b+$'
    'b$'
    'a$'
    '^[ab]+$'
    '^(aa)+$'
)

if [[ -z "${REEZ}" ]]; then
    echo "Usage: $0 <program>" >&2
    exit 1
fi

show-and-run() {
    echo '== '"$@"
    "$@"
}

echook() {
    echo -e '\e[1;32m[OK] '"$@"'\e[0m'
}

echobad() {
    echo -e '\e[1;31m[BAD] '"$@"'\e[0m'
}

failed=0
for pattern in "${PATTERNS[@]}"; do
    if diff \
        <($REEZ "$pattern" $SAMPLE 2>/dev/null) \
        <(grep -E "$pattern" $SAMPLE 2>/dev/null) \
        >/dev/null 2>&1
    then
        echook "$pattern"
    else
        echobad "$pattern"
        show-and-run $REEZ "$pattern" $SAMPLE
        show-and-run grep -E "$pattern" $SAMPLE
        failed=1
    fi
done

if (( failed == 1 )); then
    echobad 'failed at least one test'
    exit 1
fi

echook 'passed all tests'
