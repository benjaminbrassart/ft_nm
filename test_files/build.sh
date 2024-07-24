#!/usr/bin/env sh

# shellcheck shell=dash

set -e

# c++ -c -g3 mangled.cpp -o mangled-32le.cpp.o -m32
c++ -c -g3 mangled.cpp -o mangled-64le.cpp.o

cc -c -g3 gnu-constructor.c -o gnu-constructor.c.o

rust_targets="i686-unknown-linux-gnu x86_64-unknown-linux-gnu powerpc64-unknown-linux-gnu"

for target in ${rust_targets}; do
    rustc --emit=obj -C debuginfo=full -C strip=none --target="${target}" hello.rs -o "hello-${target}.rs.o"
done
