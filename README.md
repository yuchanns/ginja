# ginja

A Go binding for [minijinja](https://github.com/mitsuhiko/minijinja), featuring a custom C layer designed for Go integration.

## Overview

Ginja provides Go developers with access to the minijinja template engine while addressing some integration challenges found in existing solutions.

## Motivation

The [official C-binding of minijinja](https://github.com/mitsuhiko/minijinja/tree/main/minijinja-cabi) uses **TLS** (Thread Local Storage) for error handling, which requires `runtime.LockOSThread` when used with Go. This approach can complicate error management and may impact Go's concurrency patterns.

The [official minijinja-go](https://github.com/mitsuhiko/minijinja/tree/main/minijinja-go) relies on **CGO**, which introduces build complexity and additional dependencies.

## Features

Ginja aims to provide a more straightforward Go integration by:

- **TLS-free C layer**: Implements a custom C interface that avoids Thread Local Storage, allowing better integration with Go's concurrency model
- **CGO-free approach**: Uses **purego + libffi** to eliminate CGO dependency, simplifying the build process and maintaining Go's cross-compilation capabilities
- **Go-idiomatic API**: Designed to feel natural within Go codebases

