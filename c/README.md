## Build

- To build the library and header file.
    ```bash
    mkdir -p build && cd build
    cmake ..
    make
    ```
- The header file `minijinja.h` is under `./include`
- The library is under `./target/debug` after building.
- To build and run the tests. (Note that you need to install Valgrind and GTest)
    ```bash
    ./test.sh
    ```
- To run the tests on Windows:
    ```powershell
    .\Test.ps1
    ```
