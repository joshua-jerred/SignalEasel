#!/bin/bash

mkdir -p build
cd build

# Delete old coverage file if it exists
rm -f coverage_data.*

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Debug -DSIGNALEASEL_COVERAGE=ON

# Build
cmake --build . --target signal_easel_unit_tests

# Run the unit tests
ctest --force-new-ctest-process

# Generate coverage report. Create an LCOV report file and print a text summary
gcovr -r .. --config ../project/gcovr.cfg --lcov coverage_data.lcov --txt coverage_data.txt
cat coverage_data.txt