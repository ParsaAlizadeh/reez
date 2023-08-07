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
)

if [[ -z "${REEZ}" ]]; then
    echo "Usage: $0 <program>" >&2
    exit 1
fi

failed=0
for pattern in "${PATTERNS[@]}"; do
    if diff <($REEZ $pattern $SAMPLE 2>/dev/null) <(grep -E $pattern $SAMPLE 2>/dev/null) &>/dev/null; then
        echo "[OK] $pattern"
    else
        echo "[BAD] $pattern"
        set -x
        $REEZ $pattern $SAMPLE
        grep -E $pattern $SAMPLE
        set +x
        failed=1
    fi
done

if (( failed == 1 )); then
    echo '[BAD] failed at least one test' >&2
    exit 1
fi

echo '[OK] passed all tests'
