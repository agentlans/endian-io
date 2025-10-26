# Endian I/O Utility Library

A lightweight C library for reading and writing binary files in **big-endian** and **little-endian** formats.  
It provides convenient, type-safe functions for serializing and deserializing arrays of numeric types.

## Features

- Read and write data in both **big-endian** (BE) and **little-endian** (LE) byte orders.  
- Type-safe wrappers for all common integer and floating-point types.  
- Cross-platform compatible and works seamlessly with both C and C++.  
- Simple and minimal dependency — only uses `<stdio.h>` and `<stdint.h>`.

## Header Overview

The main header file:  
```
endian_io.h
```

It declares generic endian read/write functions:

```c
int write_be(FILE* file, const uint8_t* data, size_t num, size_t size);
int read_be(FILE* file, uint8_t* data, size_t num, size_t size);
int write_le(FILE* file, const uint8_t* data, size_t num, size_t size);
int read_le(FILE* file, uint8_t* data, size_t num, size_t size);
```

Along with type-safe wrappers generated via macros for:

- `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`
- `int8_t`, `int16_t`, `int32_t`, `int64_t`
- `float`, `double`

Examples:
```c
int write_uint32_t_be(FILE* file, const uint32_t* arr, size_t n);
int read_double_le(FILE* file, double* arr, size_t n);
```

## Usage Example

Include `endian_io.h` and compile with `endian_io.c`

```c
#include "endian_io.h"

int main() {
    // Important: open file in binary mode
    FILE* f = fopen("output.bin", "wb");
    if (!f) return -1;

    uint32_t values[3] = {0x11223344, 0x55667788, 0x99AABBCC};
    // Write three uint32_t values to file in big endian format
    if (write_uint32_be(f, values, 3) != 0)
        return -1;

    fclose(f);

    f = fopen("output.bin", "rb");
    // Read back the three uint32_t values from the file
    uint32_t read_back[3];
    if (read_uint32_be(f, read_back, 3) != 0)
        return -1;

    fclose(f);
    return 0;
}
```

## Function Return Values

- `0`: Success  
- `-1`: I/O error or failure in reading/writing bytes

## Licence

MIT License — free for personal and commercial use.  
Include attribution in your source or documentation.

