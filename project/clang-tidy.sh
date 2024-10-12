#!/bin/bash

FILES=()

shopt -s nullglob

for file in $(find src -name "*.cpp"); do
  FILES+=($file)
done

python3 ./project/run-clang-tidy.py -config-file=.clang-tidy -p=build -use-color=true

# for file in $(find src -name "*.cpp"); do
#   # clang-tidy -config-file=.clang-tidy $file -p=build
# done

# FILE_LIST=(
  # "src/modulator.cpp"
# )

# clang-tidy -config-file=.clang-tidy $(echo ${FILE_LIST[*]}) -p=build