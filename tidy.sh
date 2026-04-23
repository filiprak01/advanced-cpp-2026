#!/bin/bash
if [ ! -f build/compile_commands.json ]; then
    echo "Brak compile_commands.json. Uruchom najpierw: mkdir -p build && cd build && cmake .."
    exit 1
fi
find common/src server/src src -name '*.cpp' | xargs clang-tidy -p build