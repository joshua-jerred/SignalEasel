#!/bin/bash

python3 ./project/run-clang-tidy.py -config-file=.clang-tidy -p=build \
-use-color=true -source-filter='.*SignalEasel/src*.'