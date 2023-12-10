#!/bin/bash

FILES=()

shopt -s nullglob

for file in $(find src -name "*.cpp"); do
  FILES+=($file)
done

clang-tidy -config-file=.clang-tidy $(echo ${FILES[*]}) -p=build --header-filter=src

# for file in $(find src -name "*.cpp"); do
#   # clang-tidy -config-file=.clang-tidy $file -p=build
# done

# FILE_LIST=(
  # "src/modulator.cpp"
# )

# clang-tidy -config-file=.clang-tidy $(echo ${FILE_LIST[*]}) -p=build