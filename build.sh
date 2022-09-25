#!/bin/bash

echo "Building the project..."

if [ ! -d "./build" ]; then
    ./build_from_scratch.sh $@
fi

cd build

num_procs=$(nproc)
echo "building using ${num_procs} workers"
if make -j${num_procs}; then
echo "build ready";
else 
echo "build failed!";
cd ..
exit 2
fi

cd ..
