#!/usr/bin/env sh

# shellcheck enable=style
# shellcheck shell=dash

test_count=0

test_nm() {
    test_count=$((test_count + 1))

    printf -- '\n============ TEST %d ============\n\n' "${test_count}"
    printf -- '  ARGS: %s\n\n' "$(echo "$@" | xargs)"

    nm "$@" > "logs/${test_count}.stdout.nm.log" 2> "logs/${test_count}.stderr.nm.log"
    ./ft_nm "$@" > "logs/${test_count}.stdout.ft.log" 2> "logs/${test_count}.stderr.ft.log"

    git --no-pager diff --no-index --word-diff=color --word-diff-regex=. "logs/${test_count}.stdout.ft.log" "logs/${test_count}.stdout.nm.log"
    git --no-pager diff --no-index --word-diff --word-diff-regex=. "logs/${test_count}.stdout.ft.log" "logs/${test_count}.stdout.nm.log" > "logs/${test_count}.diff"

    printf -- '\n=================================\n\n'
}

mkdir -p logs

test_nm
test_nm a.out

test_nm --
test_nm -- a.out

test_nm -a
test_nm -a a.out

test_nm -a --
test_nm -a -- a.out
