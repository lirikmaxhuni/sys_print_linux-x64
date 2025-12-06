#include "print.h"
#include <stdio.h>

int main(void)
{
    char array[] = "Hello, World!";
    int status = print(array, sizeof(array) - 1);

    if (status < 0) {
        perror("print failed");
        return 1;
    }

    return 0;
}
