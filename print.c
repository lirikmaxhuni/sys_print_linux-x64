#include "print.h"
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <stddef.h>

#define PRINT_MAX_CHUNK 1024

int print(const char *array, size_t length)
{
    if (!array) {
        errno = EINVAL;
        return -1;
    }
    if (length == 0)
        return 0;

    size_t total_written = 0;
    int result = 0;
    char buffer[PRINT_MAX_CHUNK + 1];

    while (total_written < length) {
        size_t chunk = length - total_written;
        if (chunk > PRINT_MAX_CHUNK)
            chunk = PRINT_MAX_CHUNK;

        for (size_t i = 0; i < chunk; ++i)
            buffer[i] = array[total_written + i];

        int add_newline = 0;
        if (total_written + chunk == length) {
            buffer[chunk++] = '\n';
            add_newline = 1;
        }

        size_t written = 0;
        while (written < chunk) {
            long ret = syscall(SYS_write, 1, buffer + written, chunk - written);

            if (ret < 0) {
                if (errno == EINTR)
                    continue;
                result = -2;
                goto cleanup;
            }

            if (ret == 0) {
                errno = EIO;
                result = -3;
                goto cleanup;
            }

            written += (size_t)ret;
        }

        total_written += (add_newline ? (chunk - 1) : chunk);
    }

cleanup:
    for (size_t i = 0; i < sizeof(buffer); ++i)
        buffer[i] = 0;

    return result;
}
