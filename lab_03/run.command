#!/bin/sh
set -eu
cd -- "$(dirname -- "$0")"
mkdir -p build-run
"${CXX:-c++}" -std=c++20 -Wall -Wextra -Wpedantic \
    main.cpp book.cpp book_set.cpp -o build-run/lab03
exec ./build-run/lab03
