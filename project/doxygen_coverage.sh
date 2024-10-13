#!/bin/bash

set -e

# https://github.com/psycofdj/coverxygen

DOXYFILE="project/doxygen/Doxyfile_coverage"
SRC_DIR="src"
DOXYGEN_OUTPUT_DIR="build/dox_coverage/xml"
DOC_COVERAGE_FILE="build/doc-coverage.info"

# Remove the old coverage data file and doxygen output if they exists
rm -f $DOC_COVERAGE_FILE
rm -rf $DOXYGEN_OUTPUT_DIR

# Create the output directory
mkdir -p $DOXYGEN_OUTPUT_DIR

# Generate the XML
doxygen $DOXYFILE

# Generate the coverage data
python3 -m coverxygen --xml-dir $DOXYGEN_OUTPUT_DIR --src-dir $SRC_DIR --output $DOC_COVERAGE_FILE --format summary

# Print the report for CI/CD
cat $DOC_COVERAGE_FILE

# BUFFER = 31.9% (284/891)
BUFFER=$(tail -1 $DOC_COVERAGE_FILE | cut -d ":" -f 2 | awk '{$1=$1;print}')

# COVERAGE_LABEL = "DOC_COVERAGE_LABEL=31.9% (284/891)"
# COVERAGE_VAR = "DOC_COVERAGE=31.9"
if [ ! -z "$GITHUB_ACTIONS" ]; then
  echo "DOC_COVERAGE_LABEL=$BUFFER" >> $GITHUB_ENV
  echo "DOC_COVERAGE_VALUE=$(echo $BUFFER | cut -d "%" -f 1)" >> $GITHUB_ENV
fi