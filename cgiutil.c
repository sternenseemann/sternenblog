#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stringutil.h"

void send_header(char key[], char val[]) {
    fputs(key, stdout);
    fputs(": ", stdout);
    fputs(val, stdout);
    puts("\r");
}

void terminate_headers(void) {
    puts("\r");
}

char *http_status_line(int status) {
    switch(status) {
        case 200:
            return "200 OK";
        case 400:
            return "400 Bad Request";
        case 401:
            return "401 Unauthorized";
        case 403:
            return "403 Forbidden";
        case 404:
            return "404 Not Found";
        default:
            // default to 500
            return "500 Internal Server Error";
    }
}

int http_errno(int err) {
    switch(err) {
        case EACCES:
            return 403;
        case ENOENT:
            return 404;
        case ENOTDIR:
            return 404;
        default:
            return 500;
    }
}

int urlencode_realloc(char **input, int size) {
    if(*input == NULL || size <= 0) {
        return -1;
    }

    int output_size = size;
    char *output = malloc(output_size);
    int output_pos = 0;

    if(output == NULL) {
        return -1;
    }

    for(int i = 0; i < size; i++) {
        char c = *(*input + i);
        bool needs_escape;
        switch(c) {
            // generic delimiters
            // we assume we never need to escape '/'. This
            // should hold since on unix filenames won't
            // contain slashes and the basis for all URLs
            // in sternenblog are actual files
            case ':': case '?': case '#': case '[': case ']': case '@':
            // sub delimiters
            case '!': case '$': case '&': case '\'': case '(': case ')':
            case '*': case '+': case ',': case ';': case '=':
            // other characters to encode
            case '%': case ' ':
                needs_escape = 1;
                break;
            // in order to simplify the code we just assume
            // everything else doesn't have to be encoded
            //
            // otherwise we'd need to be UTF-8 aware here
            // and consider more than one byte at a time.
            default:
                needs_escape = 0;
        }

        int necessary_space = needs_escape ? 3 : 1;

        if(output_pos + necessary_space >= output_size) {
            output_size += necessary_space;
            char *tmp = realloc(output, output_size);
            if(tmp == NULL) {
                free(output);
                return -1;
            } else {
                output = tmp;
            }
        }

        if(needs_escape) {
            short a = (c & 0xf0) >> 4;
            short b = c & 0x0f;
            output[output_pos++] = '%';
            output[output_pos++] = nibble_hex(a);
            output[output_pos++] = nibble_hex(b);
        } else {
            output[output_pos++] = c;
        }
    }

    free(*input);
    *input = output;

    return output_size;
}
