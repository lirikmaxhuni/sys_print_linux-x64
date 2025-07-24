# sys_print_linux_x64

A robust, safe, direct-syscall print function for Linux x86-64 in C.

## Features

- Directly uses the Linux `write` syscall (no libc dependency)
- Handles partial writes, EINTR, and errors robustly
- Limits stack usage (prints in chunks)
- Zeroes stack buffer to avoid information leaks
- Returns detailed error codes and sets `errno`
- Appends a newline to output

## Limitations

- **Linux x86-64 only** (uses inline assembly and syscall numbers specific to this platform)
- Bypasses stdio buffering and redirection
- No formatting (not a `printf` replacement)
- Prints embedded nulls as-is

## Usage

```c
#include <stddef.h>
#include <errno.h>

// Paste the print() function here

int main() {
    char msg[] = "Hello, World!";
    int status = print(msg, sizeof(msg) - 1);
    if (status < 0) {
        // Handle error
    }
    return 0;
}
```

## Compilation

```sh
gcc -o print print.c
```

## License

MIT License
