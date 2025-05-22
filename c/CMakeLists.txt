cmake_minimum_required(VERSION 3.24)
cmake_policy(SET CMP0135 NEW)

project(minijinja-c)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Enable AddressSanitizer for tests
option(TEST_ENABLE_ASAN "Enable AddressSanitizer for tests" OFF)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set C++ standard for GoogleTest (requires C++14 or higher)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set cargo build directory based on build type
set(CARGO_DIST_DIR "${PROJECT_SOURCE_DIR}/target/debug")
if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CARGO_BUILD_TYPE "--release")
    set(CARGO_DIST_DIR "${PROJECT_SOURCE_DIR}/target/release")
endif()

# Set library paths
set(MINIJINJA_STATIC_LIB "${CARGO_DIST_DIR}/libminijinja_c${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(MINIJINJA_SHARED_LIB "${CARGO_DIST_DIR}/libminijinja_c${CMAKE_SHARED_LIBRARY_SUFFIX}")
message(NOTICE "-- Minijinja C static lib: ${MINIJINJA_STATIC_LIB}")
message(NOTICE "-- Minijinja C shared lib: ${MINIJINJA_SHARED_LIB}")

# Custom target for cargo build
add_custom_target(cargo_build
    COMMAND sh -c "cargo build ${CARGO_BUILD_TYPE}"
    BYPRODUCTS ${MINIJINJA_STATIC_LIB} ${MINIJINJA_SHARED_LIB}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

# CMake target for static lib
add_library(minijinja_c_static INTERFACE)
target_link_libraries(minijinja_c_static INTERFACE ${MINIJINJA_STATIC_LIB})
target_include_directories(minijinja_c_static INTERFACE include)
add_dependencies(minijinja_c_static cargo_build)

# CMake target for shared lib
add_library(minijinja_c_shared INTERFACE)
target_link_libraries(minijinja_c_shared INTERFACE ${MINIJINJA_SHARED_LIB})
target_include_directories(minijinja_c_shared INTERFACE include)
add_dependencies(minijinja_c_shared cargo_build)

# Fetch GoogleTest
include(FetchContent)

set(GOOGLETEST_VERSION 1.15.2)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v${GOOGLETEST_VERSION}.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test targets
file(GLOB TEST_SRCS tests/*.cpp)
add_executable(tests ${TEST_SRCS})
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(tests PRIVATE minijinja_c_shared gtest_main)

# Add AddressSanitizer if enabled
if (TEST_ENABLE_ASAN)
    target_compile_options(tests PRIVATE -fsanitize=address)
    target_link_options(tests PRIVATE -fsanitize=address)
endif()

# Enable testing
enable_testing()
add_test(NAME minijinja_tests COMMAND tests)

