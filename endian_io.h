#ifndef ENDIAN_IO_H
#define ENDIAN_IO_H

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Writes an array of elements to a file in big-endian byte order.
 *
 * This function converts integers to big-endian order as needed before writing.
 *
 * @param file  Open binary file for writing.
 * @param data  Pointer to input data array.
 * @param num   Number of elements to write.
 * @param size  Size of each element in bytes.
 * @return 0 on success, -1 on error.
 */
int write_be(FILE* file, const uint8_t* data, size_t num, size_t size);

/**
 * @brief Reads an array of elements from a file written in big-endian order.
 *
 * This function reads data and converts it from big-endian to host order.
 *
 * @param file  Open binary file for reading.
 * @param data  Pointer to output buffer.
 * @param num   Number of elements to read.
 * @param size  Size of each element in bytes.
 * @return 0 on success, -1 on error.
 */
int read_be(FILE* file, uint8_t* data, size_t num, size_t size);

/**
 * @brief Writes an array of elements to a file in little-endian byte order.
 *
 * @param file  Open binary file for writing.
 * @param data  Pointer to input data array.
 * @param num   Number of elements to write.
 * @param size  Size of each element in bytes.
 * @return 0 on success, -1 on error.
 */
int write_le(FILE* file, const uint8_t* data, size_t num, size_t size);

/**
 * @brief Reads an array of elements from a file written in little-endian order.
 *
 * @param file  Open binary file for reading.
 * @param data  Pointer to output buffer.
 * @param num   Number of elements to read.
 * @param size  Size of each element in bytes.
 * @return 0 on success, -1 on error.
 */
int read_le(FILE* file, uint8_t* data, size_t num, size_t size);


// Generic macro to declare endian functions for any number type
#define DECLARE_ENDIAN_IO_FUNCS(NUMBERTYPE, ENDIAN) \
    int write_##NUMBERTYPE##_##ENDIAN(FILE* file, const NUMBERTYPE* arr, size_t n); \
    int read_##NUMBERTYPE##_##ENDIAN(FILE* file, NUMBERTYPE* arr, size_t n);

// Declare functions for all needed types
DECLARE_ENDIAN_IO_FUNCS(uint8_t, be)
DECLARE_ENDIAN_IO_FUNCS(uint16_t, be)
DECLARE_ENDIAN_IO_FUNCS(uint32_t, be)
DECLARE_ENDIAN_IO_FUNCS(uint64_t, be)
DECLARE_ENDIAN_IO_FUNCS(int8_t, be)
DECLARE_ENDIAN_IO_FUNCS(int16_t, be)
DECLARE_ENDIAN_IO_FUNCS(int32_t, be)
DECLARE_ENDIAN_IO_FUNCS(int64_t, be)
DECLARE_ENDIAN_IO_FUNCS(float, be)
DECLARE_ENDIAN_IO_FUNCS(double, be)

DECLARE_ENDIAN_IO_FUNCS(uint8_t, le)
DECLARE_ENDIAN_IO_FUNCS(uint16_t, le)
DECLARE_ENDIAN_IO_FUNCS(uint32_t, le)
DECLARE_ENDIAN_IO_FUNCS(uint64_t, le)
DECLARE_ENDIAN_IO_FUNCS(int8_t, le)
DECLARE_ENDIAN_IO_FUNCS(int16_t, le)
DECLARE_ENDIAN_IO_FUNCS(int32_t, le)
DECLARE_ENDIAN_IO_FUNCS(int64_t, le)
DECLARE_ENDIAN_IO_FUNCS(float, le)
DECLARE_ENDIAN_IO_FUNCS(double, le)

#ifdef __cplusplus
}
#endif

#endif // ENDIAN_IO_H

