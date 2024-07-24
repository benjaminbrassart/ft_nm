#!/usr/bin/env sh

# shellcheck enable=style
# shellcheck shell=dash

test_count=0
status=0
use_valgrind=

if [ -n "${USE_VALGRIND}" ]; then
    if valgrind_version="$(valgrind --version | grep -Eo '[0-9]+\.[0-9]+\.[0-9]+')"; then
        major="$(echo "${valgrind_version}" | cut -d '.' -f 1)"
        minor="$(echo "${valgrind_version}" | cut -d '.' -f 2)"

        if [ "${major}" -gt 3 ] || [ "${major}" -eq 3 ] && [ "${minor}" -ge 24 ]; then
            use_valgrind='yes'
        else
            printf 'Error: valgrind version is < 3.24.0\n' >&2
            exit 1
        fi
    else
        printf 'Warning: could not parse valgrind version (is it in PATH?)\n' >&2
        exit 1
    fi
fi

pid="$$"
psub_dir="$(mktemp -d -- "tmp.psub.${pid}.XXXXXXXXXX")"

trap 'rm -rf -- "${psub_dir}"' EXIT

# fish-like process substitution function for posix shell
psub() {
    tmpdir="$(mktemp -d -p "${psub_dir}")" || return
    tmp="${tmpdir}/tmp" || return

    echo "${tmp}"
    cat > "${tmp}"
}

convert_error_message() {
    sed -e 's/^ft_nm/nm/' \
        -e 's/No such file or directory/No such file/' \
        -re 's/cannot open ([^:]+): No such file/'\''\1'\'': No such file/' \
        -re 's/cannot open ([^:]+)/\1/' \
        -re 's/cannot map ([^:]+): Is a directory/Warning: '\''\1'\'' is a directory/'
}

test_nm() {
    test_count=$((test_count + 1))
    mkdir -p "logs/${test_count}"
    args="$(echo "$@" | xargs)"

    printf -- '\n============ TEST %d ============\n\n' "${test_count}"
    printf -- '  ARGS: %s\n\n' "${args}"

    echo "${args}" > "logs/${test_count}/command.log"

    nm "$@" \
        > "logs/${test_count}/stdout.nm.log" \
        2> "logs/${test_count}/stderr.nm.log"
    exit_nm="$?"
    echo "${exit_nm}" > "logs/${test_count}/exit.nm.log"

    if [ -n "${use_valgrind}" ]; then
        valgrind \
            --error-exitcode=42 \
            --track-fds=yes \
            --show-leak-kinds=all \
            --leak-check=full \
            --xml=yes \
            --xml-file="logs/${test_count}/valgrind.ft.xml" \
            ./ft_nm "$@"
    else
        ./ft_nm "$@"
    fi > "logs/${test_count}/stdout.ft.log" 2> "logs/${test_count}/stderr.ft.log"

    exit_ft="$?"
    echo "${exit_ft}" > "logs/${test_count}/exit.ft.log"

    git --no-pager diff --no-prefix --no-index --word-diff=color --word-diff-regex=. \
        "logs/${test_count}/stdout.ft.log" \
        "logs/${test_count}/stdout.nm.log"
    git --no-pager diff --no-prefix --no-index \
        "logs/${test_count}/stdout.ft.log" \
        "logs/${test_count}/stdout.nm.log" \
        > "logs/${test_count}/stdout.diff"

    git --no-pager diff --no-prefix --no-index --word-diff=color --word-diff-regex=. \
        "$(< logs/${test_count}/stderr.ft.log convert_error_message | psub)" \
        "logs/${test_count}/stderr.nm.log"
    git --no-pager diff --no-prefix --no-index \
        "$(< logs/${test_count}/stderr.ft.log convert_error_message | psub)" \
        "logs/${test_count}/stderr.nm.log" \
        > "logs/${test_count}/stderr.diff"

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

rm -rf logs
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

test_nm -- ft_nm
test_nm -- ft_nm ft_nm

test_nm -u -- ft_nm
test_nm -u -- ft_nm ft_nm

test_nm -g -- ft_nm
test_nm -g -- ft_nm ft_nm

test_nm -gu -- ft_nm
test_nm -gu -- ft_nm ft_nm

test_nm -ug -- ft_nm
test_nm -ug -- ft_nm ft_nm

test_nm -ug -u -- ft_nm
test_nm -ug -u -- ft_nm ft_nm

test_nm -agurp -- ft_nm ft_nm

test_nm -- *.o
test_nm -a -- *.o
test_nm -u -- *.o
test_nm -g -- *.o
test_nm -ga -- *.o
test_nm -au -- *.o
test_nm -a -- libnm.so

test_nm -- test_files/*.o
test_nm -a test_files/*.o

test_nm /bin/ls
test_nm /bin/*

exit "${status}"
