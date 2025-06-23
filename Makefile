# Licensed under the MIT License
# Copyright (c) 2024 ginja project contributors
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

.PHONY: tests clean c-tests bench help check-deps

# =============================================================================
# Platform Detection
# =============================================================================

# Detect current OS and architecture
OS := $(shell uname -s | tr '[:upper:]' '[:lower:]')
ARCH := $(shell uname -m)

# Normalize architecture names
ifeq ($(ARCH),x86_64)
    ARCH := amd64
endif
ifeq ($(ARCH),aarch64)
    ARCH := arm64
endif

# Define file extensions for different OS
ifeq ($(OS),linux)
    LIB_PREFIX := lib
    LIB_EXT := so
else ifeq ($(OS),darwin)
    LIB_PREFIX := lib
    LIB_EXT := dylib
else
    LIB_PREFIX := 
    LIB_EXT := dll
    OS := windows
endif

# =============================================================================
# Build Configuration
# =============================================================================

# Check if RELEASE_MODE is set
ifeq ($(RELEASE_MODE),1)
    BUILD_MODE := release
    BUILD_FLAG := --release
else
    BUILD_MODE := debug
    BUILD_FLAG := 
endif

# Library file paths
C_LIB_DIR := c/target/$(BUILD_MODE)
C_LIB_FILE := $(C_LIB_DIR)/$(LIB_PREFIX)minijinja_c.$(LIB_EXT)
EMBED_DIR := internal/embed
COMPRESSED_LIB := $(EMBED_DIR)/$(LIB_PREFIX)minijinja_c.$(OS).$(ARCH).$(LIB_EXT).zst

# Source files that trigger rebuilds
RUST_SOURCES := $(wildcard c/src/*.rs) c/Cargo.toml

# =============================================================================
# Dependency Checks
# =============================================================================

check-deps:
	@echo "Checking required dependencies..."
	@command -v cargo >/dev/null 2>&1 || { echo "Error: cargo is required but not installed"; exit 1; }
	@command -v rustc >/dev/null 2>&1 || { echo "Error: rustc is required but not installed"; exit 1; }
	@command -v go >/dev/null 2>&1 || { echo "Error: go is required but not installed"; exit 1; }
	@command -v zstd >/dev/null 2>&1 || { echo "Error: zstd is required but not installed"; exit 1; }
	@echo "All dependencies are available"

# =============================================================================
# Build Targets
# =============================================================================

# Build C library for current platform
$(C_LIB_FILE): $(RUST_SOURCES) | check-deps
	@echo "Building Rust library ($(BUILD_MODE)) for $(OS)/$(ARCH)..."
	@echo "Compiling with cargo $(BUILD_FLAG)..."
	cd c && cargo build $(BUILD_FLAG)

# Create compressed library (depends on RELEASE_MODE)
$(COMPRESSED_LIB): $(C_LIB_FILE)
	@echo "Compressing $(BUILD_MODE) library for $(OS)/$(ARCH)..."
	@mkdir -p $(EMBED_DIR)
	zstd -19 $(C_LIB_FILE) -o $(COMPRESSED_LIB)
	@echo "$(BUILD_MODE) library compressed and saved to $(COMPRESSED_LIB)"


# =============================================================================
# Test Targets
# =============================================================================

# Run Go tests (supports RELEASE_MODE=1 for release build)
tests: $(COMPRESSED_LIB)
	@echo "Running Go tests in $(BUILD_MODE) mode..."
	go test -race -gcflags=all=-d=checkptr -v .


# Run C tests
c-tests: check-deps
	@echo "Building and running C tests..."
	@mkdir -p c/build
	cd c/build && cmake .. -DTEST_ENABLE_ASAN=ON
	cd c/build && make tests

# Run benchmarks
bench: $(COMPRESSED_LIB)
	@echo "Running Go benchmarks..."
	go test -bench=. -benchmem -count=6 -run=^$$ -v

# =============================================================================
# Maintenance Targets
# =============================================================================

# Clean all build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@if [ -d "c/target" ]; then cd c && cargo clean; fi
	@rm -rf c/build
	@rm -f $(EMBED_DIR)/*.so.zst $(EMBED_DIR)/*.dylib.zst $(EMBED_DIR)/*.dll.zst
	@go clean -testcache
	@echo "Clean completed"

# Show help information
help:
	@echo "Ginja Makefile - Build system for Go Jinja2 templating library"
	@echo ""
	@echo "Available targets:"
	@echo "  tests          - Run Go tests (default: debug build)"
	@echo "  c-tests        - Build and run C tests"
	@echo "  bench          - Run Go benchmarks (builds current platform automatically)"
	@echo "  clean          - Clean all build artifacts"
	@echo "  check-deps     - Check if all required dependencies are installed"
	@echo "  help           - Show this help message"
	@echo ""
	@echo "Environment variables:"
	@echo "  RELEASE_MODE=1 - Use release build instead of debug build"
	@echo ""
	@echo "Current platform: $(OS)/$(ARCH)"
	@echo ""
	@echo "Dependencies:"
	@echo "  - cargo (Rust toolchain)"
	@echo "  - go (Go toolchain)"
	@echo "  - zstd (compression tool)"
	@echo "  - cmake (for C tests)"
	@echo ""
	@echo "Examples:"
	@echo "  make tests                    # Run tests in debug mode"
	@echo "  RELEASE_MODE=1 make tests     # Run tests in release mode"
	@echo "  make bench                    # Run benchmarks"
	@echo "  make clean                    # Clean build files"

