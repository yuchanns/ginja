#!/bin/bash

# Exit on error
set -e

# Function to check if build directory exists
check_and_create_build_dir() {
    if [ ! -d "build" ]; then
        echo "Creating build directory..."
        mkdir -p build
    else
        echo "Build directory already exists"
    fi
}

# Main build process
main() {
    # Create build directory if it doesn't exist
    check_and_create_build_dir

    # Change to build directory
    cd build || exit 1

    # Run cmake
    echo "Running cmake..."
    cmake ..

    # Run make test
    echo "Running make test..."
    make tests

    # Run tests
    echo "Running tests..."
    ./tests
}

# Execute main function
main

