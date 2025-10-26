#include <stddef.h>
#include <errno.h>
//#include <stdio.h>

// Max chunk size for stack buffer
#define PRINT_MAX_CHUNK 1024

// Returns 0 on success, negative on error
int print(const char *array, size_t length) {
    if (!array) {
        errno = EINVAL;
        return -1; // Null pointer
    }
    if (length == 0) {
        errno = 0;
        return 0; // Nothing to print
    }

    size_t total_written = 0;
    int result = 0;
    char buffer[PRINT_MAX_CHUNK + 1]; // +1 for newline

    while (total_written < length) {
        size_t chunk = length - total_written;
        if (chunk > PRINT_MAX_CHUNK)
            chunk = PRINT_MAX_CHUNK;

        // Copy chunk to buffer
        for (size_t i = 0; i < chunk; ++i)
            buffer[i] = array[total_written + i];
        // Add newline only to the last chunk
        if (total_written + chunk == length)
            buffer[chunk++] = '\n';

        size_t written = 0;
        while (written < chunk) {
            long ret;
            asm volatile (
                "movq $1, %%rax\n"
                "movq $1, %%rdi\n"
                "movq %1, %%rsi\n"
                "movq %2, %%rdx\n"
                "syscall\n"
                : "=a"(ret)
                : "r"(buffer + written), "r"(chunk - written)
                : "%rdi", "%rsi", "%rdx"
            );
            if (ret < 0) {
                int err = -ret;
                if (err == EINTR) {
                    continue; // Retry on interrupt
                }
                errno = err;
                result = -2; // Write error
                goto cleanup;
            }
            written += (size_t)ret;
        }
        total_written += chunk - 1; // -1 because we added newline only to last chunk
    }

cleanup:
    // Zero out buffer to avoid information leak
    for (size_t i = 0; i < sizeof(buffer); ++i)
        buffer[i] = 0;
    return result;
}

int main() {
    char array[] = "Hello, World!";
    int status = print(array, sizeof(array) - 1); // -1 to exclude null terminator
    //int status = print(NULL, 10);
    if (status < 0) {
        //handle error	
    }
    return 0;
}