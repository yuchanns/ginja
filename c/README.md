# MiniJinja C Bindings

A C binding for the [MiniJinja](https://github.com/mitsuhiko/minijinja) template engine. This library provides a C API for C/C++ applications without using Thread Local Storage.

## Features

- **TLS-free design**: No Thread Local Storage dependencies
- **C API**: Access to MiniJinja's template rendering functionality
- **Value system**: Support for strings, integers, floats, booleans, and nested data structures

## Example

Here's a simple example of using the MiniJinja C bindings:

```c
#include "minijinja.h"
#include <stdio.h>

int main() {
    // Create a new environment
    struct mj_result_env_new env_result = mj_env_new();
    if (!env_result.env) {
        printf("Failed to create environment\n");
        return 1;
    }
    struct mj_env *env = env_result.env;
    
    // Add a template
    struct mj_result_env_add_template add_result = mj_env_add_template(
        env, "hello", "Hello, {{ name }}! You are {{ age }} years old."
    );
    if (add_result.error) {
        printf("Template error: %s\n", add_result.error->message);
        mj_error_free(add_result.error);
        mj_env_free(env);
        return 1;
    }
    
    // Create context values
    struct mj_value *context = mj_value_new();
    mj_value_set_string(context, "name", "Alice");
    mj_value_set_int32(context, "age", 30);
    
    // Render the template
    struct mj_result_env_render_template render_result = mj_env_render_template(
        env, "hello", context
    );
    if (render_result.error) {
        printf("Render error: %s\n", render_result.error->message);
        mj_error_free(render_result.error);
    } else {
        printf("Result: %s\n", render_result.result);
        mj_str_free(render_result.result);
    }
    
    // Cleanup
    mj_value_free(context);
    mj_env_free(env);
    return 0;
}
```

### Working with Arrays

```c
// Create an array of strings
const char* fruits[] = {"apple", "banana", "cherry"};
mj_value_set_list_string(context, "fruits", fruits, 3);

// Create an array of integers
int64_t numbers[] = {1, 2, 3, 4, 5};
mj_value_set_list_int(context, "numbers", numbers, 5);

// Template: "Fruits: {% for fruit in fruits %}{{ fruit }}{% endfor %}"
```

### Nested Objects

```c
// Create a nested value
struct mj_value *user = mj_value_new();
mj_value_set_string(user, "name", "Bob");
mj_value_set_int32(user, "age", 25);

// Add the nested value to the main context
mj_value_set_value(context, "user", user);

// Template: "User: {{ user.name }} ({{ user.age }})"
mj_value_free(user);
```

## Prerequisites

- **CMake 3.24+**
- **C11 and C++14 compatible compiler** (GCC, Clang, MSVC)
- **Rust toolchain** (for building the underlying library)
- **GoogleTest** (automatically downloaded for testing)
- To format the code, you need to install clang-format
    - The `minijinja.h` is not formatted by hands when you contribute, please do not format the file. Use make format only.
    - If your contribution is related to the files under ./tests, you may format it before submitting your pull request. But notice that different versions of clang-format may format the files differently.
- (optional) Doxygen need to be installed to generate documentations.

For Ubuntu and Debian:
```bash
# install C/C++ toolchain
sudo apt install -y build-essential

# install clang-format
sudo apt install clang-format

# install and build GTest library under /usr/lib and softlink to /usr/local/lib
sudo apt-get install libgtest-dev

# install CMake
sudo apt-get install cmake

# install Rust
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

## Build

### Basic Build

1. **Clone and navigate to the C bindings directory:**
   ```bash
   git clone https://github.com/yuchanns/ginja.git
   cd ginja/c
   ```

2. **Create build directory and configure:**
   ```bash
   mkdir build && cd build
   cmake ..
   ```

3. **Build the library:**
   ```bash
   make  # Linux/macOS
   # or
   cmake --build .  # Cross-platform
   ```

### Build Outputs

After building, you'll find:
- **Header file**: `./include/minijinja.h`
- **Static library**: `./target/debug/libminijinja_c.a` (Linux/macOS)
- **Shared library**: `./target/debug/libminijinja_c.so` (Linux), `./target/debug/libminijinja_c.dylib` (macOS)

### Release Build

For optimized release builds:
```bash
mkdir build-release && cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### Running Tests

The project includes comprehensive C++ tests using GoogleTest:

1. **Build with tests:**
   ```bash
   cd build
   cmake ..
   make tests
   ```

2. **Run tests:**
   ```bash
   ctest
   # or directly run the test executable
   ./tests
   ```

3. **Run tests with AddressSanitizer (Linux/macOS only):**
   ```bash
   cmake -DTEST_ENABLE_ASAN=ON ..
   make tests
   ./tests
   ```

### CMake Integration

To use MiniJinja C bindings in your CMake project:

```cmake
# Add the MiniJinja C bindings subdirectory
add_subdirectory(path/to/minijinja-c)

# Link to your target
target_link_libraries(your_target PRIVATE minijinja_c_static)
# or for shared library
target_link_libraries(your_target PRIVATE minijinja_c_shared)
```

## Documentation

### API Reference

The documentation index page source is under ./docs/doxygen/html/index.html. If you want to build the documentations yourself, you could use

```bash
# this requires you to install doxygen
make doc
```

The complete API is documented in the header file `include/minijinja.h` with detailed Doxygen comments. Key components include:

#### Core Types
- `mj_env`: Template environment for managing templates
- `mj_value`: Context values for template rendering
- `mj_error`: Error information with categorized error codes

#### Environment Functions
- `mj_env_new()`: Create a new template environment
- `mj_env_add_template()`: Add a template to the environment
- `mj_env_render_template()`: Render a template with context
- `mj_env_render_named_string()`: Render template source directly

#### Value Functions
- `mj_value_new()`: Create a new value container
- `mj_value_set_*()`: Set various data types (string, int, float, bool, arrays)
- `mj_value_set_list_*()`: Set arrays of various types

#### Memory Management
- `mj_env_free()`: Free environment resources
- `mj_value_free()`: Free value resources
- `mj_error_free()`: Free error resources
- `mj_str_free()`: Free strings returned by the library

### Error Handling

The library uses structured error handling with specific error codes:

```c
if (result.error) {
    switch (result.error->code) {
        case MJ_TEMPLATE_NOT_FOUND:
            printf("Template not found: %s\n", result.error->message);
            break;
        case MJ_SYNTAX_ERROR:
            printf("Template syntax error: %s\n", result.error->message);
            break;
        default:
            printf("Error: %s\n", result.error->message);
    }
    mj_error_free(result.error);
}
```

### Configuration Options

Environment behavior can be customized:

```c
// Configure whitespace handling
mj_env_set_trim_blocks(env, true);
mj_env_set_lstrip_blocks(env, true);
mj_env_set_keep_trailing_newline(env, false);

// Set undefined variable behavior
mj_env_set_undefined_behavior(env, MJ_UNDEFINED_BEHAVIOR_STRICT);

// Configure recursion limits
mj_env_set_recursion_limit(env, 500);

// Enable debug mode
mj_env_set_debug(env, true);
```

## License

Licensed under the Apache License, Version 2.0. See [LICENSE](../LICENSE) for details.

---

**Note**: This C binding is part of the [Ginja](https://github.com/yuchanns/ginja) project, which provides Go bindings for MiniJinja using a TLS-free approach.
