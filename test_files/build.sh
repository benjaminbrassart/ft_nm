#!/usr/bin/env sh

# shellcheck shell=dash

errors=0

inc_errors() {
    errors=$((errors + 1))
}

c++ -c -g3 mangled.cpp -o mangled-32le.cpp.o -m32 || inc_errors
c++ -c -g3 mangled.cpp -o mangled-64le.cpp.o || inc_errors

c++ -c -g3 -O0 oop-nonsense.cpp -o oop-nonsense-32le.cpp.o -m32 || inc_errors
c++ -c -g3 -O0 oop-nonsense.cpp -o oop-nonsense-64le.cpp.o || inc_errors

cc -c -g3 gnu-constructor.c -o gnu-constructor.c.o || inc_errors
cc -c -fcommon common.c -o common.o || inc_errors

# i686      => 32-bit little-endian
# x86_64    => 64-bit little-endian
# powerpc   => 32-bit big-endian
# powerpc64 => 64-bit big-endian
rust_targets="i686-unknown-linux-gnu x86_64-unknown-linux-gnu powerpc-unknown-linux-gnu powerpc64-unknown-linux-gnu x86_64-unknown-freebsd i686-unknown-freebsd"

for target in ${rust_targets}; do
    rustc --emit=obj -C debuginfo=full -C strip=none --target="${target}" hello.rs -o "hello-${target}.rs.o" || inc_errors
done

if [ "${errors}" -ne 0 ]; then
    printf -- '\n--> Summary: %d errors\n' "${errors}" >&2
    exit 1
fi
