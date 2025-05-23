# Stop on any error
$ErrorActionPreference = "Stop"

# Create build directory if it doesn't exist
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# Change to build directory
Push-Location "build"


try {
    # Configure with CMake
    cmake -G "Visual Studio 17 2022" ..
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configure failed with exit code $LASTEXITCODE"
    }

    # Build with CMake
    cmake --build . --config Debug
    if ($LASTEXITCODE -ne 0) {
        throw "CMake build failed with exit code $LASTEXITCODE"
    }

    # Run tests
    ctest -C Debug --rerun-failed --output-on-failure
    if ($LASTEXITCODE -ne 0) {
        throw "CTest failed with exit code $LASTEXITCODE"
    }
} finally {
    # Return to original directory
    Pop-Location
}

