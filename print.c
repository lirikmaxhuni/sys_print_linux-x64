#include <stddef.h>
#include <errno.h>

#define PRINT_MAX_CHUNK 1024

int print(const char *array, size_t length)
{
    if (!array) {
        errno = EINVAL;
        return -1;
    }

    if (length == 0) {
        return 0;   // Do NOT touch errno on success
    }

    size_t total_written = 0;
    int result = 0;
    char buffer[PRINT_MAX_CHUNK + 1]; // +1 for optional newline

    while (total_written < length) {

        size_t chunk = length - total_written;
        if (chunk > PRINT_MAX_CHUNK)
            chunk = PRINT_MAX_CHUNK;

        // Copy user data into local buffer
        for (size_t i = 0; i < chunk; ++i)
            buffer[i] = array[total_written + i];

        int add_newline = 0;
        if (total_written + chunk == length) {
            buffer[chunk++] = '\n';
            add_newline = 1;
        }

        size_t written = 0;
        while (written < chunk) {
            long ret;

            asm volatile (
                "movq $1, %%rax\n"   // sys_write
                "movq $1, %%rdi\n"   // stdout
                "movq %1, %%rsi\n"
                "movq %2, %%rdx\n"
                "syscall\n"
                : "=a"(ret)
                : "r"(buffer + written),
                  "r"(chunk - written)
                : "%rdi", "%rsi", "%rdx", "rcx", "r11", "memory"
            );

            if (ret < 0) {
                int err = (int)(-ret);
                if (err == EINTR)
                    continue;

                errno = err;
                result = -2;
                goto cleanup;
            }

            if (ret == 0) {   // Safety: prevent infinite loop
                errno = EIO;
                result = -3;
                goto cleanup;
            }

            written += (size_t)ret;
        }

        // Only advance by actual USER DATA, not newline
        total_written += (add_newline ? (chunk - 1) : chunk);
    }

cleanup:
    for (size_t i = 0; i < sizeof(buffer); ++i)
        buffer[i] = 0;

    return result;
}
