#define _POSIX_C_SOURCE 1
#include <time.h>
#include "timeutil.h"

extern long timezone;

char *format_string(enum time_format t, long tz) {
    switch(t) {
        case RSS_TIME_FORMAT:
            return "%a, %d %b %Y %T %z";
        case HTML_TIME_FORMAT_READABLE:
            return tz == 0 ? "%Y-%m-%d %TZ" : "%Y-%m-%d %T%z";
        case ATOM_TIME_FORMAT:
        default:
            return tz == 0 ? "%Y-%m-%dT%TZ" : "%Y-%m-%dT%T%z";
    }
}

size_t flocaltime(char *b, enum time_format type, size_t size, const time_t *time) {
    tzset();
    struct tm *local = localtime(time);
    char *format = format_string(type, timezone);

    size_t res = strftime(b, size, format, local);

    // prevent any buffer overflows
    b[size - 1] = '\0';

    return res;
}
