#!/bin/bash

# Retrieve glaze from github if not already present
if [[ ! -d "external/glaze" ]]; then
  mkdir -p external
  mkdir -p external/glaze
  curl -L \
       -O \
       --retry 3 \
       --retry-delay 5 \
       --retry-max-time 60 \
       --connect-timeout 15 \
       --fail \
       https://github.com/stephenberry/glaze/archive/refs/tags/v6.5.1.zip
  unzip v6.5.1.zip -d .
  mv glaze-6.5.1/* external/glaze
  rm v6.5.1.zip
  rm -r glaze-6.5.1
fi

# Make build directory if it doesn't exist
mkdir -p build

FLAGS="-std=c++23 -I./ -I./external/glaze/include -I./pkg -O3 -DNDEBUG -march=native"

# Build script for the C++ project
# Compile each source file into an object file
g++ $FLAGS -c main.cpp -o build/main.o
g++ $FLAGS -c pkg/component/protocol.cpp -o build/protocol.o
g++ $FLAGS -c pkg/component/prototype.cpp -o build/prototype.o
g++ $FLAGS -c pkg/component/device.cpp -o build/device.o
g++ $FLAGS -c pkg/component/message.cpp -o build/message.o
g++ $FLAGS -c pkg/element/checksum.cpp -o build/checksum.o
g++ $FLAGS -c pkg/element/segment.cpp -o build/segment.o
g++ $FLAGS -c pkg/observe/statistics.cpp -o build/statistics.o
g++ $FLAGS -c pkg/observe/logger.cpp -o build/logger.o
g++ $FLAGS -c pkg/transport/simulation.cpp -o build/simulation.o
g++ $FLAGS -c pkg/transform/conversion.cpp -o build/conversion.o
# Link all object files into the final executable
g++ $FLAGS build/main.o build/protocol.o build/prototype.o build/device.o build/message.o build/checksum.o build/segment.o build/statistics.o build/logger.o build/simulation.o build/conversion.o -o donp

#Clean up
rm -r build

exit 0
# End of build script