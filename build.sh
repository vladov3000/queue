#!/usr/bin/env sh
set -e
clang -g -O2 queue.c test.c -o test
