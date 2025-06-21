.PHONY: tests clean build-current c_tests

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

# Define file extensions and prefixes for different OS
ifeq ($(OS),linux)
    LIB_PREFIX := lib
    LIB_EXT := so
    RUST_TARGET_SUFFIX := 
endif
ifeq ($(OS),darwin)
    LIB_PREFIX := lib
    LIB_EXT := dylib
    RUST_TARGET_SUFFIX := 
endif
ifeq ($(OS),mingw64_nt)
    OS := windows
    LIB_PREFIX := 
    LIB_EXT := dll
    RUST_TARGET_SUFFIX := .exe
endif
ifeq ($(findstring mingw,$(OS)),mingw)
    OS := windows
    LIB_PREFIX := 
    LIB_EXT := dll
    RUST_TARGET_SUFFIX := .exe
endif
ifeq ($(findstring msys,$(OS)),msys)
    OS := windows
    LIB_PREFIX := 
    LIB_EXT := dll
    RUST_TARGET_SUFFIX := .exe
endif

# Define Rust targets for cross-compilation
RUST_TARGET_linux_amd64 := x86_64-unknown-linux-gnu
RUST_TARGET_linux_arm64 := aarch64-unknown-linux-gnu
RUST_TARGET_darwin_amd64 := x86_64-apple-darwin
RUST_TARGET_darwin_arm64 := aarch64-apple-darwin
RUST_TARGET_windows_amd64 := x86_64-pc-windows-gnu
RUST_TARGET_windows_arm64 := aarch64-pc-windows-gnu

# Current target based on detected OS and ARCH
CURRENT_TARGET := $(RUST_TARGET_$(OS)_$(ARCH))

# Define library names for different platforms
CURRENT_LIB_FILE := $(LIB_PREFIX)minijinja_c.$(LIB_EXT)
CURRENT_COMPRESSED_FILE := $(LIB_PREFIX)minijinja_c.$(OS).$(ARCH).$(LIB_EXT).zst

# Cross-compilation targets
define build_target
c/target/$(2)/debug/$(3): c/src/*.rs c/Cargo.toml
	@echo "Cross-compiling Rust library for $(1)..."
	cd c && rustup target add $(2) && cargo build --target $(2)

internal/embed/$(4): c/target/$(2)/debug/$(3)
	@echo "Compressing library for $(1) with zstd..."
	cd c && zstd -19 target/$(2)/debug/$(3) -o $(4)
	@echo "Moving compressed library to embed directory..."
	mv c/$(4) internal/embed/
endef

# Generate build rules for all supported platforms
$(eval $(call build_target,linux/amd64,$(RUST_TARGET_linux_amd64),libminijinja_c.so,libminijinja_c.linux.amd64.so.zst))
$(eval $(call build_target,linux/arm64,$(RUST_TARGET_linux_arm64),libminijinja_c.so,libminijinja_c.linux.arm64.so.zst))
$(eval $(call build_target,darwin/amd64,$(RUST_TARGET_darwin_amd64),libminijinja_c.dylib,libminijinja_c.darwin.amd64.dylib.zst))
$(eval $(call build_target,darwin/arm64,$(RUST_TARGET_darwin_arm64),libminijinja_c.dylib,libminijinja_c.darwin.arm64.dylib.zst))
$(eval $(call build_target,windows/amd64,$(RUST_TARGET_windows_amd64),minijinja_c.dll,minijinja_c.windows.amd64.dll.zst))
$(eval $(call build_target,windows/arm64,$(RUST_TARGET_windows_arm64),minijinja_c.dll,minijinja_c.windows.arm64.dll.zst))

# Build current platform
build-current: internal/embed/$(CURRENT_COMPRESSED_FILE)
	@echo "Built library for current platform: $(OS)/$(ARCH)"

# Run tests with current platform library
tests: build-current
	@echo "Running Go tests..."
	go test -v .

clean:
	@echo "Cleaning build artifacts..."
	cd c && cargo clean
	rm -rf c/build
	rm -f internal/embed/*.so.zst internal/embed/*.dylib.zst internal/embed/*.dll.zst
	go clean -testcache

c_tests:
	@echo "Building and running C tests..."
	mkdir -p c/build
	cd c/build && cmake .. -DTEST_ENABLE_ASAN=ON
	cd c/build && make tests

