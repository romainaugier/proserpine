#!/bin/bash

# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 - Present Romain Augier MIT License
# All rights reserved

CONFIG=RelWithDebInfo
RMBUILD=0

parse_args()
{
    [[ "$1" == "clean" ]] && RMBUILD=1
}

for arg in "$@"
do
    parse_args "$arg"
done

if [[ -d build && $RMBUILD -eq 1 ]]; then
    echo Removing old build directory
    rm -rf build
fi

cmake -S . -B build

if [[ $? -ne 0 ]]; then
    echo Error during CMake configuration
    exit 1
fi

cd build

cmake --build . --config $CONFIG

if [[ $? -ne 0 ]]; then
    echo Error during CMake build
    exit 1
fi

ctest --output-on-failure -C $CONFIG

if [[ $? -ne 0 ]]; then
    cd ..
    echo Error during CMake test
    exit 1
fi

echo -e "\nBuild and test successful!"

cd ..

exit 0
