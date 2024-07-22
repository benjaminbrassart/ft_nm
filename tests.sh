#!/usr/bin/env sh

# shellcheck enable=style
# shellcheck shell=dash

test_count=0
status=0

test_nm() {
    test_count=$((test_count + 1))
    args="$(echo "$@" | xargs)"

    printf -- '\n============ TEST %d ============\n\n' "${test_count}"
    printf -- '  ARGS: %s\n\n' "${args}"

    echo "${args}" > "logs/${test_count}.command.log"

    nm "$@" \
        > "logs/${test_count}.stdout.nm.log" \
        2> "logs/${test_count}.stderr.nm.log"
    exit_nm="$?"
    echo "${exit_nm}" > "logs/${test_count}.exit.nm.log"

    ./ft_nm "$@" \
        > "logs/${test_count}.stdout.ft.log" \
        2> "logs/${test_count}.stderr.ft.log"
    exit_ft="$?"
    echo "${exit_ft}" > "logs/${test_count}.exit.ft.log"

    git --no-pager diff --no-index --word-diff=color --word-diff-regex=. \
        "logs/${test_count}.stdout.ft.log" \
        "logs/${test_count}.stdout.nm.log"
    git --no-pager diff --no-index \
        "logs/${test_count}.stdout.ft.log" \
        "logs/${test_count}.stdout.nm.log" \
        > "logs/${test_count}.diff"

    current_status="$?"

    if [ "${current_status}" -ne "0" ]; then
        status="1"
        printf -- "\n  KO: check diff\n"
    elif [ "${exit_nm}" -ne "${exit_ft}" ]; then
        status="1"
        printf -- "\n  KO: exit status (nm=%d, ft=%d)\n" "${exit_nm}" "${exit_ft}"
    else
        printf -- "  OK\n"
    fi

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

test_nm -r --
test_nm -r -- a.out

test_nm -p --
test_nm -p -- a.out

test_nm -rp --
test_nm -rp -- a.out

test_nm -rpa -- ft_nm

test_nm -a --
test_nm -a -- ft_nm
test_nm -a -- does_not_exist
test_nm -a -- does_not_exist a.out
test_nm -a -- does_not_exist ft_nm
test_nm -a -- ft_nm ft_nm

test_nm $(seq 255)
test_nm $(seq 256)

exit "${status}"
