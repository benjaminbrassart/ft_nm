#!/usr/bin/env sh

# shellcheck shell=dash

set -e

c++ -c -g3 mangled.cpp -o mangled-32le.cpp.o -m32
c++ -c -g3 mangled.cpp -o mangled-64le.cpp.o

cc -c -g3 gnu-constructor.c -o gnu-constructor.c.o

# i686      => 32-bit little-endian
# x86_64    => 64-bit little-endian
# powerpc   => 32-bit big-endian
# powerpc64 => 64-bit big-endian
rust_targets="i686-unknown-linux-gnu x86_64-unknown-linux-gnu powerpc-unknown-linux-gnu powerpc64-unknown-linux-gnu x86_64-unknown-freebsd i686-unknown-freebsd"

for target in ${rust_targets}; do
    rustc --emit=obj -C debuginfo=full -C strip=none --target="${target}" hello.rs -o "hello-${target}.rs.o"
done
