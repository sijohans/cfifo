#!/bin/sh
rm -rf build
mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Debug -DSANITIZE_ADDRESS=On -DSANITIZE_UNDEFINED=On ..
make
make test

lcov --base-directory .. --directory .. --capture --output-file coverage.info
genhtml -o . coverage.info