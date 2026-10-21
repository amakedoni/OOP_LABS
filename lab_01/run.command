#!/bin/sh
set -eu
cd -- "$(dirname -- "$0")"
mkdir -p build
"${CXX:-c++}" -std=c++20 -Wall -Wextra -Wpedantic \
    main.cpp array_ops.cpp -o build/lab01
exec ./build/lab01
