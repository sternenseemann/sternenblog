#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

char nibble_hex(short h) {
    switch(h) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            return (h + 48);
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            return (h + 55);
        default:
            return 0;
    }
}

char *catn_alloc(size_t n, ...) {
    va_list args;
    size_t pos = 0;
    char *buffer = NULL;
    size_t buffer_size = 0;
    va_start(args, n);

    for(size_t i = 0; i < n; i++) {
        char *str = va_arg(args, char *);
        size_t copy_len = strlen(str) + (i + 1 == n ? 1 : 0);

        char *tmp = realloc(buffer, buffer_size + copy_len);

        if(tmp == NULL) {
            break;
        } else {
            buffer = tmp;
        }

        buffer_size += copy_len;
        memcpy(buffer + pos, str, copy_len);
        pos += copy_len;
    }

    if(buffer != NULL) {
        // ensure it's NUL terminated
        buffer[buffer_size - 1] = '\0';
    }

    va_end(args);

    return buffer;
}

