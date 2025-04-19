# ginja

A Go binding for [minijinja](https://github.com/mitsuhiko/minijinja), with an optimized C layer.

## Motivation

The [official C-binding of minijinja](https://github.com/mitsuhiko/minijinja/tree/main/minijinja-cabi) uses **TLS** (Thread Local Storage) for error handling, which requires `runtime.LockOSThread` when used with Go. This can impact performance and complicate error management.

ginja provides a more idiomatic Go experience by implementing a custom C layer that avoids TLS, enabling seamless integration with Go's concurrency model.
