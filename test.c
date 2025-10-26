#include "endian_io.h"
#include <stdlib.h>
#include <string.h>

int main(void) {
    const char *filename_be = "test_be.bin";
    const char *filename_le = "test_le.bin";

    uint32_t data_out[4] = {0x11223344, 0xAABBCCDD, 0x12345678, 0xDEADBEEF};
    uint32_t data_in[4]  = {0};

    FILE *f_be = fopen(filename_be, "wb");
    FILE *f_le = fopen(filename_le, "wb");
    if (!f_be || !f_le) {
        perror("fopen");
        return -1;
    }

    // Write in big-endian format
    if (write_uint32_t_be(f_be, data_out, 4) != 0) {
        fprintf(stderr, "Failed to write big-endian data\n");
    }

    // Write in little-endian format
    if (write_uint32_t_le(f_le, data_out, 4) != 0) {
        fprintf(stderr, "Failed to write little-endian data\n");
    }

    fclose(f_be);
    fclose(f_le);

    // Read them back
    f_be = fopen(filename_be, "rb");
    f_le = fopen(filename_le, "rb");
    if (!f_be || !f_le) {
        perror("fopen");
        return -1;
    }

    if (read_uint32_t_be(f_be, data_in, 4) == 0) {
        printf("Big-endian read:\n");
        for (int i = 0; i < 4; ++i)
            printf(" 0x%08X", data_in[i]);
        printf("\n");
    }

    memset(data_in, 0, sizeof(data_in));

    if (read_uint32_t_le(f_le, data_in, 4) == 0) {
        printf("Little-endian read:\n");
        for (int i = 0; i < 4; ++i)
            printf(" 0x%08X", data_in[i]);
        printf("\n");
    }

    fclose(f_be);
    fclose(f_le);

    return 0;
}
