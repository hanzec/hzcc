#!/bin/sh

# build tempary directory
mkdir -m 700 /dev/shm/hanzech_build_cache

# configure the projdect
cmake -S . -B /dev/shm/hanzech_build_cache

# compile project
cmake --build /dev/shm/hanzech_build_cache --target mycc documentation -- -j$(nproc)

# build product directory
mkdir build

# copy documentation
cp -r /dev/shm/hanzech_build_cache/Documentation/*.pdf ./build/

# copy executables
cp -r /dev/shm/hanzech_build_cache/bin/* ./build/

# delete tempary directory
rm -rf /dev/shm/hanzech_build_cache
