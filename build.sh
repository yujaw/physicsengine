#!/bin/bash

echo "Building the project..."
make

if [ $? -eq 0 ]; then
    echo "Build successful. Running the program..."
    ./build/main
else
    echo "Build failed. Please check the errors."
fi