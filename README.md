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

## Usage

```bash
go get -u go.yuchanns.xyz/ginja
```

### Basic Example

```go
package main

import (
    "fmt"
    "log"
    
    "go.yuchanns.xyz/ginja"
)

func main() {
    // Create a new environment
    env, err := ginja.New()
    if err != nil {
        log.Fatal(err)
    }
    defer env.Close()
    
    // Add a template
    err = env.AddTemplate("hello", "Hello, {{ name }}!")
    if err != nil {
        log.Fatal(err)
    }
    
    // Render the template
    result, err := env.RenderTemplate("hello", map[string]any{
        "name": "World",
    })
    if err != nil {
        log.Fatal(err)
    }
    
    fmt.Println(result) // Output: Hello, World!
}
```

### Working with Structures

```go
type Person struct {
    Name string
    Age  int
}

type Address struct {
    Street string
    City   string
}

type Employee struct {
    Person
    Address Address
    ID      int
}

// Add template
env.AddTemplate("employee", `
Employee: {{ emp.Name }} (ID: {{ emp.ID }})
Age: {{ emp.Age }}
Address: {{ emp.Address.Street }}, {{ emp.Address.City }}
`)

// Render with struct data
result, err := env.RenderTemplate("employee", map[string]any{
    "emp": Employee{
        Person:  Person{Name: "John Doe", Age: 30},
        Address: Address{Street: "123 Main St", City: "New York"},
        ID:      12345,
    },
})
```

### Working with Slices and Maps

```go
// Template with slice iteration
env.AddTemplate("users", `
Users:
{% for user in users %}
  - {{ user.Name }} ({{ user.Age }} years old)
{% endfor %}
`)

// Data with slice of structs
users := []Person{
    {Name: "Alice", Age: 25},
    {Name: "Bob", Age: 30},
    {Name: "Charlie", Age: 35},
}

result, err := env.RenderTemplate("users", map[string]any{
    "users": users,
})

// Template with map data
env.AddTemplate("config", `
Configuration:
Database: {{ config.database.host }}:{{ config.database.port }}
Redis: {{ config.redis.host }}:{{ config.redis.port }}
`)

// Data with nested maps
config := map[string]any{
    "database": map[string]any{
        "host": "localhost",
        "port": 5432,
    },
    "redis": map[string]any{
        "host": "127.0.0.1",
        "port": 6379,
    },
}

result, err := env.RenderTemplate("config", map[string]any{
    "config": config,
})
```

### Conditionals and Filters

```go
// Template with conditionals and filters
env.AddTemplate("product", `
Product: {{ product.name }}
Price: ${{ product.price }}
{% if product.in_stock %}
  Status: In Stock ({{ product.quantity }} available)
{% else %}
  Status: Out of Stock
{% endif %}
Items count: {{ items|length }}
`)

// Data with mixed types
result, err := env.RenderTemplate("product", map[string]any{
    "product": map[string]any{
        "name":     "Laptop",
        "price":    999.99,
        "in_stock": true,
        "quantity": 10,
    },
    "items": []string{"item1", "item2", "item3"},
})
```

### Advanced Usage with Any Types

```go
// Template handling mixed data types
env.AddTemplate("mixed", `
Data Analysis:
{% for item in data %}
  {% if item.Name %}
    User: {{ item.Name }} (ID: {{ item.ID }})
  {% else %}
    Value: {{ item }}
  {% endif %}
{% endfor %}
`)

// Mixed slice with different types
data := []any{
    Person{Name: "Alice", Age: 25},
    "Simple String",
    42,
    true,
    map[string]string{"key": "value"},
}

result, err := env.RenderTemplate("mixed", map[string]any{
    "data": data,
})
```

## Development

### Running Tests
To run the Go-binding tests, use the following command:

```bash
make tests
```

To run the C-binding tests, use:

```bash
make c_tests
```

