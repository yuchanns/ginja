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

# MSVC specific settings
if(MSVC)
    # Set runtime library
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # Configure warning levels
    add_compile_options(/W4 /wd4100)
endif()

# Set cargo build directory based on build type
set(CARGO_DIST_DIR "${PROJECT_SOURCE_DIR}/target/debug")
if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CARGO_BUILD_TYPE "--release")
    set(CARGO_DIST_DIR "${PROJECT_SOURCE_DIR}/target/release")
endif()

# Set library paths based on platform
if(WIN32)
    set(MINIJINJA_IMPORT_LIB "${CARGO_DIST_DIR}/minijinja_c${CMAKE_IMPORT_LIBRARY_SUFFIX}")  # .lib
    set(MINIJINJA_SHARED_LIB "${CARGO_DIST_DIR}/minijinja_c${CMAKE_SHARED_LIBRARY_SUFFIX}")  # .dll
    message(NOTICE "-- Minijinja C import lib: ${MINIJINJA_IMPORT_LIB}")
    message(NOTICE "-- Minijinja C shared lib: ${MINIJINJA_SHARED_LIB}")
else()
    set(MINIJINJA_STATIC_LIB "${CARGO_DIST_DIR}/libminijinja_c${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(MINIJINJA_SHARED_LIB "${CARGO_DIST_DIR}/libminijinja_c${CMAKE_SHARED_LIBRARY_SUFFIX}")
    message(NOTICE "-- Minijinja C static lib: ${MINIJINJA_STATIC_LIB}")
    message(NOTICE "-- Minijinja C shared lib: ${MINIJINJA_SHARED_LIB}")
endif()

# Custom target for cargo build
if (WIN32)
    # Add extra cargo flags for Windows MSVC
    set(CARGO_FLAGS "")
    if(MSVC)
        set(CARGO_FLAGS "--target x86_64-pc-windows-msvc")
    endif()
    
    add_custom_target(cargo_build
        COMMAND cmd /c "cargo build ${CARGO_BUILD_TYPE} ${CARGO_FLAGS}"
        BYPRODUCTS ${MINIJINJA_IMPORT_LIB} ${MINIJINJA_SHARED_LIB}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
else()
    add_custom_target(cargo_build
        COMMAND sh -c "cargo build ${CARGO_BUILD_TYPE}"
        BYPRODUCTS ${MINIJINJA_STATIC_LIB} ${MINIJINJA_SHARED_LIB}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
endif()

# CMake target for static/import lib
if(WIN32)
    add_library(minijinja_c_static SHARED IMPORTED GLOBAL)
    set_target_properties(minijinja_c_static PROPERTIES
        IMPORTED_IMPLIB "${MINIJINJA_IMPORT_LIB}"
        IMPORTED_LOCATION "${MINIJINJA_SHARED_LIB}"
    )
    target_include_directories(minijinja_c_static INTERFACE include)
    add_dependencies(minijinja_c_static cargo_build)
else()
    add_library(minijinja_c_static INTERFACE)
    target_link_libraries(minijinja_c_static INTERFACE ${MINIJINJA_STATIC_LIB})
    target_include_directories(minijinja_c_static INTERFACE include)
    add_dependencies(minijinja_c_static cargo_build)
endif()

# CMake target for shared lib
if(WIN32)
    add_library(minijinja_c_shared SHARED IMPORTED GLOBAL)
    set_target_properties(minijinja_c_shared PROPERTIES
        IMPORTED_IMPLIB "${MINIJINJA_IMPORT_LIB}"
        IMPORTED_LOCATION "${MINIJINJA_SHARED_LIB}"
    )
    target_include_directories(minijinja_c_shared INTERFACE include)
    add_dependencies(minijinja_c_shared cargo_build)
else()
    add_library(minijinja_c_shared INTERFACE)
    target_link_libraries(minijinja_c_shared INTERFACE ${MINIJINJA_SHARED_LIB})
    target_include_directories(minijinja_c_shared INTERFACE include)
    add_dependencies(minijinja_c_shared cargo_build)
endif()

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
target_link_libraries(tests PRIVATE 
    minijinja_c_shared 
    gtest_main
    # Add Windows system libraries
    $<$<PLATFORM_ID:Windows>:ws2_32>    # Windows Sockets API
    $<$<PLATFORM_ID:Windows>:userenv>   # User Profile API
    $<$<PLATFORM_ID:Windows>:ntdll>     # NT API
    $<$<PLATFORM_ID:Windows>:bcrypt>    # Cryptography API
)

# Copy DLL to test output directory on Windows
if(WIN32)
    add_custom_command(TARGET tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MINIJINJA_SHARED_LIB}"
            $<TARGET_FILE_DIR:tests>
    )
endif()

# Add AddressSanitizer if enabled
if (TEST_ENABLE_ASAN)
    if(MSVC)
        message(WARNING "AddressSanitizer is not supported with MSVC")
    else()
        target_compile_options(tests PRIVATE -fsanitize=address)
        target_link_options(tests PRIVATE -fsanitize=address)
    endif()
endif()

# Enable testing
enable_testing()
add_test(NAME minijinja_tests COMMAND tests)

