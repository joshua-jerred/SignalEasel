#!/bin/bash

EXPORT_CHANGES=""
if [ "$1" == "-e" ]; then
  echo "EXPORTING CHANGES"
  EXPORT_CHANGES="-export-fixes=.clang-tidy-suggested-fixes.yaml"
fi

cd build && make compile_commands.json && cd ..

python3 ./project/run-clang-tidy.py -config-file=.clang-tidy -p=build \
-use-color=true -source-filter='.*SignalEasel/src*.' $EXPORT_CHANGES