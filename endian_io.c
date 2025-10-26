#include "endian_io.h"
#include <string.h>
#include <stdlib.h>

// Internal endian enumeration
typedef enum {
    ENDIAN_LITTLE,
    ENDIAN_BIG
} endian_t;

// -----------------------------------------------------------------------------
// Endianness Detection
// -----------------------------------------------------------------------------
static inline int is_little_endian(void) {
    uint16_t x = 1;
    return *((uint8_t*)&x) == 1;
}

// -----------------------------------------------------------------------------
// Byte-Swap Helpers
// -----------------------------------------------------------------------------
static inline uint16_t bswap16(uint16_t x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap16(x);
#else
    return (x >> 8) | (x << 8);
#endif
}

static inline uint32_t bswap32(uint32_t x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap32(x);
#else
    return ((x >> 24) & 0xFF) |
           ((x >> 8)  & 0xFF00) |
           ((x << 8)  & 0xFF0000) |
           ((x << 24) & 0xFF000000);
#endif
}

static inline uint64_t bswap64(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap64(x);
#else
    return ((x >> 56) & 0xFFULL) |
           ((x >> 40) & 0xFF00ULL) |
           ((x >> 24) & 0xFF0000ULL) |
           ((x >> 8)  & 0xFF000000ULL) |
           ((x << 8)  & 0xFF00000000ULL) |
           ((x << 24) & 0xFF0000000000ULL) |
           ((x << 40) & 0xFF000000000000ULL) |
           ((x << 56) & 0xFF00000000000000ULL);
#endif
}

static inline void reverse_bytes(uint8_t* data, size_t size) {
    for (size_t i = 0; i < size / 2; i++) {
        uint8_t tmp = data[i];
        data[i] = data[size - 1 - i];
        data[size - 1 - i] = tmp;
    }
}

// -----------------------------------------------------------------------------
// Core Endian Conversion
// -----------------------------------------------------------------------------
static inline void convert_endian(void* elem, size_t size) {
    switch (size) {
        case 2: *(uint16_t*)elem = bswap16(*(uint16_t*)elem); break;
        case 4: *(uint32_t*)elem = bswap32(*(uint32_t*)elem); break;
        case 8: *(uint64_t*)elem = bswap64(*(uint64_t*)elem); break;
        default: reverse_bytes((uint8_t*)elem, size); break;
    }
}

// -----------------------------------------------------------------------------
// Generic File I/O (Private)
// -----------------------------------------------------------------------------

static int write_endian(FILE* file, const uint8_t* data,
                        size_t num, size_t size, endian_t target_endian) {
    if (!file || !data || size == 0 || num == 0)
        return -1;

    const int host_little = is_little_endian();
    const int swap_needed = (host_little && target_endian == ENDIAN_BIG) ||
                            (!host_little && target_endian == ENDIAN_LITTLE);

    // Fast path: same endianness → direct block write
    if (!swap_needed)
        return fwrite(data, size, num, file) == num ? 0 : -1;

    // Choose a conservative, portable alignment
    const size_t buffer_size = 256;
    const size_t alignment = 16; // safe for most SIMD / hardware
    uint8_t* buffer = NULL;

#if defined(_ISOC11_SOURCE)
    buffer = aligned_alloc(alignment, buffer_size);
#elif defined(_POSIX_VERSION)
    if (posix_memalign((void**)&buffer, alignment, buffer_size) != 0)
        buffer = NULL;
#else
    // Fallback: use local array (may be unaligned, still safe)
    uint8_t stack_buffer[buffer_size];
    buffer = stack_buffer;
#endif

    if (!buffer)
        return -1;

    const size_t block_elems = buffer_size / size;

    for (size_t offset = 0; offset < num; ) {
        size_t batch = num - offset < block_elems ? num - offset : block_elems;

        // Copy + endian swap into buffer
        for (size_t i = 0; i < batch; i++) {
            uint8_t* dst = buffer + i * size;
            const uint8_t* src = data + (offset + i) * size;
            memcpy(dst, src, size);
            convert_endian(dst, size);
        }

        if (fwrite(buffer, size, batch, file) != batch) {
#if !defined(_ISOC11_SOURCE) && !defined(_POSIX_VERSION)
            // No free needed for stack buffer
#else
            free(buffer);
#endif
            return -1;
        }

        offset += batch;
    }

#if defined(_ISOC11_SOURCE) || defined(_POSIX_VERSION)
    free(buffer);
#endif

    return 0;
}


static int read_endian(FILE* file, uint8_t* data,
                       size_t num, size_t size, endian_t source_endian) {
    if (!file || !data || size == 0)
        return -1;

    const int host_little = is_little_endian();
    const int swap_needed = (host_little && source_endian == ENDIAN_BIG) ||
                            (!host_little && source_endian == ENDIAN_LITTLE);

    for (size_t i = 0; i < num; i++) {
        uint8_t* dst = data + i * size;
        if (fread(dst, 1, size, file) != size)
            return -1;
        if (swap_needed)
            convert_endian(dst, size);
    }
    return 0;
}

// -----------------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------------
int write_be(FILE* file, const uint8_t* data, size_t num, size_t size) {
    return write_endian(file, data, num, size, ENDIAN_BIG);
}

int read_be(FILE* file, uint8_t* data, size_t num, size_t size) {
    return read_endian(file, data, num, size, ENDIAN_BIG);
}

int write_le(FILE* file, const uint8_t* data, size_t num, size_t size) {
    return write_endian(file, data, num, size, ENDIAN_LITTLE);
}

int read_le(FILE* file, uint8_t* data, size_t num, size_t size) {
    return read_endian(file, data, num, size, ENDIAN_LITTLE);
}


// Helper macro to generate implementations
#define DEFINE_ENDIAN_IO_FUNCS(NUMBERTYPE, ENDIAN) \
int write_##NUMBERTYPE##_##ENDIAN(FILE* file, const NUMBERTYPE* arr, size_t n) { \
    return write_##ENDIAN(file, (const uint8_t*)arr, n, sizeof(NUMBERTYPE)); \
} \
int read_##NUMBERTYPE##_##ENDIAN(FILE* file, NUMBERTYPE* arr, size_t n) { \
    return read_##ENDIAN(file, (uint8_t*)arr, n, sizeof(NUMBERTYPE)); \
}

// Big-endian versions
DEFINE_ENDIAN_IO_FUNCS(uint8_t, be)
DEFINE_ENDIAN_IO_FUNCS(uint16_t, be)
DEFINE_ENDIAN_IO_FUNCS(uint32_t, be)
DEFINE_ENDIAN_IO_FUNCS(uint64_t, be)
DEFINE_ENDIAN_IO_FUNCS(int8_t, be)
DEFINE_ENDIAN_IO_FUNCS(int16_t, be)
DEFINE_ENDIAN_IO_FUNCS(int32_t, be)
DEFINE_ENDIAN_IO_FUNCS(int64_t, be)
DEFINE_ENDIAN_IO_FUNCS(float, be)
DEFINE_ENDIAN_IO_FUNCS(double, be)

// Little-endian versions
DEFINE_ENDIAN_IO_FUNCS(uint8_t, le)
DEFINE_ENDIAN_IO_FUNCS(uint16_t, le)
DEFINE_ENDIAN_IO_FUNCS(uint32_t, le)
DEFINE_ENDIAN_IO_FUNCS(uint64_t, le)
DEFINE_ENDIAN_IO_FUNCS(int8_t, le)
DEFINE_ENDIAN_IO_FUNCS(int16_t, le)
DEFINE_ENDIAN_IO_FUNCS(int32_t, le)
DEFINE_ENDIAN_IO_FUNCS(int64_t, le)
DEFINE_ENDIAN_IO_FUNCS(float, le)
DEFINE_ENDIAN_IO_FUNCS(double, le)
