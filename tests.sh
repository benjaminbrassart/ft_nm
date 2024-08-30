#!/usr/bin/env sh

# shellcheck enable=style
# shellcheck shell=dash

#
# USAGE:
#
# This script has several configuration options you may enable or disable
# to test specific behaviors. Here are the currently supported options:
#
# CHECK_STDERR: string
#   Set to non-empty to check stderr output diff.
#
# USE_VALGRIND: string
#   Set to non-empty to wrap ft_nm calls with valgrind.
#   Requires valgrind >= 3.24.0.
#
# DIFF_STYLE: enum(line|normal|word)
#   Set diff output style, defaults to "word".
#
# In addition to these switches, you may specify custom test files
# as arguments, like this:
#
# ./tests.sh -g -- /bin/ls ./foo.o
#
# All arguments are passed to ONE ft_nm invokation. It currenly is not
# possible to create multiple ft_nm calls.
#

export LANG=C

check_stderr=0
test_count=0
status=0
use_valgrind=
diff_style=word

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

if [ -n "${DIFF_STYLE}" ]; then
    case "${DIFF_STYLE}" in
        word|line)
            diff_style="${DIFF_STYLE}"
            ;;
        normal)
            diff_style="line"
            ;;
        *)
            printf -- 'Unknown diff style: %s\n' "${DIFF_STYLE}" >&2
            exit 1
    esac
fi

if [ -n "${CHECK_STDERR}" ]; then
    check_stderr=1
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

    printf -- '\n============ TEST %d ============\n\n' "${test_count}"

    if [ "$#" -le 15 ]; then
        args="$(echo "$@" | xargs)"
    printf -- '  ARGS: %s\n\n' "${args}"
    else
        printf -- '  ARGS: [%d arguments]\n\n' "$#"
    fi

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

    if [ "${diff_style}" = word ]; then
        git_diff_opts="--word-diff=color --word-diff-regex=."
    fi

    git --no-pager diff --no-prefix --no-index ${git_diff_opts} \
        "logs/${test_count}/stdout.ft.log" \
        "logs/${test_count}/stdout.nm.log"
    git --no-pager diff --no-prefix --no-index \
        "logs/${test_count}/stdout.ft.log" \
        "logs/${test_count}/stdout.nm.log" \
        > "logs/${test_count}/stdout.diff"

    status_stdout="$?"

    status_stderr=0

    if [ "${check_stderr}" -ne "0" ]; then
        git --no-pager diff --no-prefix --no-index ${git_diff_opts} \
            "$(< logs/${test_count}/stderr.ft.log convert_error_message | psub)" \
            "logs/${test_count}/stderr.nm.log"
        git --no-pager diff --no-prefix --no-index \
            "$(< logs/${test_count}/stderr.ft.log convert_error_message | psub)" \
            "logs/${test_count}/stderr.nm.log" \
            > "logs/${test_count}/stderr.diff"

        status_stderr="$?"
    fi

    ok=1

    if [ "${status_stdout}" -ne "0" ] || [ "${status_stderr}" -ne "0" ]; then
        status="1"
        ok="0"
        printf -- "\n  KO: check diff\n"
    fi

    if [ "${exit_nm}" -ne "${exit_ft}" ]; then
        if [ "${ok}" -eq "1" ]; then
            printf -- " \n"
        fi

        status="1"
        ok="0"
        printf -- "  KO: exit status (nm=%d, ft=%d)\n" "${exit_nm}" "${exit_ft}"
    fi

    if [ "${ok}" -eq "1" ]; then
        printf -- "  OK\n"
    fi

    printf -- '\n=================================\n\n'
}

rm -rf logs
mkdir -p logs

if [ "$#" -ne 0 ]; then
    test_nm "$@"
else
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
    test_nm -a -- test_files/libnm.so

    test_nm -- test_files/*.o
    test_nm -a test_files/*.o
    test_nm -a test_files/ft_nm-afl++
    test_nm -g test_files/ft_nm-afl++
    test_nm -u test_files/ft_nm-afl++
    test_nm -ur test_files/ft_nm-afl++
    test_nm -up test_files/ft_nm-afl++
    test_nm -ar test_files/ft_nm-afl++
    test_nm -ap test_files/ft_nm-afl++

    test_nm /bin/ls
    test_nm /bin/docker
fi

exit "${status}"
