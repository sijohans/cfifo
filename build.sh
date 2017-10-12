#!/bin/sh
mkdir -p _build
cd _build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
make
make test
cppcheck --project=compile_commands.json --enable=all --suppress=missingIncludeSystem --std=c89
lcov --base-directory . --directory . --capture --output-file coverage.info
genhtml -o . _build/coverage.info