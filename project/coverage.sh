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
ctest --force-new-ctest-process --quiet

# Generate coverage report. Create an LCOV report file and print a text summary
gcovr -r .. --config ../project/gcovr.cfg --lcov coverage_data.lcov --txt coverage_data.txt

# Print the report
cat coverage_data.txt

# COVERAGE_VAR = "TEST_COVERAGE=58"
export COVERAGE_VAR="TEST_COVERAGE=$(tail -2 coverage_data.txt | head -1 | awk '{print $(NF)}' | tr -d '%')"

if [ ! -z "$GITHUB_ACTIONS" ]; then
  # Send to the gha environment
  echo $COVERAGE_VAR >> $GITHUB_ENV
fi