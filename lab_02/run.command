#!/bin/sh
set -eu
cd -- "$(dirname -- "$0")"
mkdir -p build-run
"${CXX:-c++}" -std=c++20 -Wall -Wextra -Wpedantic \
    main.cpp matrix.cpp -o build-run/lab02
exec ./build-run/lab02
