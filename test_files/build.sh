#!/usr/bin/env sh

# shellcheck shell=dash

set -e

# c++ -c -g3 mangled.cpp -o mangled-32le.cpp.o -m32
c++ -c -g3 mangled.cpp -o mangled-64le.cpp.o

# rustc --emit=obj -C debuginfo=full -C strip=none --target=i686-unknown-linux-gnu hello.rs -o hello-i686-unknown-linux-gnu.rs.o
rustc --emit=obj -C debuginfo=full -C strip=none --target=x86_64-unknown-linux-gnu hello.rs -o hello-x86_64-unknown-linux-gnu.rs.o
rustc --emit=obj -C debuginfo=full -C strip=none --target=powerpc64-unknown-linux-gnu hello.rs -o hello-powerpc64-unknown-linux-gnu.rs.o
