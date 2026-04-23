#!/bin/bash
find common/src common/include server/src server/include src tests \
    -name '*.cpp' -o -name '*.hpp' | xargs clang-format -i