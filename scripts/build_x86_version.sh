#!/bin/bash

set -e

rm -rf ./my_release/x86
rm -rf ./tmp_build_dir
mkdir ./tmp_build_dir
cd ./tmp_build_dir
cmake ..
make -j4
cd ..
rm -rf ./tmp_build_dir
echo "Build completed!"
