#include <stdio.h>
#include <string.h>

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
