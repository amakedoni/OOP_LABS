#!/bin/sh
set -eu
cd "$(dirname "$0")"
mkdir -p build
"${CXX:-c++}" -std=c++20 -Wall -Wextra -Wpedantic -g \
    main.cpp array_ops.cpp -o build/lab01_main
exec ./build/lab01_main
