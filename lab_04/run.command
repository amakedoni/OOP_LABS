#!/bin/sh
set -eu
cd -- "$(dirname -- "$0")"
mkdir -p build-run
"${CXX:-c++}" -std=c++20 -Wall -Wextra -Wpedantic \
    main.cpp component.cpp collection.cpp -o build-run/lab04
exec ./build-run/lab04
