#define _POSIX_C_SOURCE 1
#define _XOPEN_SOURCE 1 // for timezone
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "timeutil.h"
#include "stringutil.h"

#include <stdio.h>

char *format_string(enum time_format t) {
    switch(t) {
        case RSS_TIME_FORMAT:
            return "%a, %d %b %Y %T %z";
        // both remaining cases still need a UTC offset
        // part at the end which is not supported by
        // strftime(3), so we do this ourselves in
        // flocaltime
        case HTML_TIME_FORMAT_READABLE:
            return "%Y-%m-%d %T";
        case ATOM_TIME_FORMAT:
        default:
            return "%Y-%m-%dT%T";
    }
}

size_t flocaltime(char *b, enum time_format type, size_t size, const time_t *time) {
    tzset();
    struct tm *local = localtime(time);
    char *format = format_string(type);

    size_t res = strftime(b, size, format, local);

    if(type == ATOM_TIME_FORMAT || type == HTML_TIME_FORMAT_READABLE) {
        // for these formats we need to append a RFC3339 UTC offset
        // unfortunately it is *not* exactly provided by strftime,
        // but in hindsight it might be better to do a little string
        // manipulation than this madness, since the libc timezone
        // API is horrible (at least POSIX / glibc)
        size_t offset_size = 7;
        char offset[offset_size];

        if(timezone == 0 && !local->tm_isdst) {
            offset[0] = 'Z';
            offset[1] = '\0';
        } else {
            // for some reason timezone is seconds *west* of UTC which
            // is inverse to how UTC offsets are denoted
            long real_offset = (-1) * timezone;

            if(daylight) {
                struct tm *local = localtime(time);

                // TODO is this correct in all cases?
                if(local->tm_isdst == 1) {
                    real_offset += 3600;
                }
            }

            char sign;
            if(real_offset > 0) {
                sign = '+';
            } else {
                sign = '-';
            }

            long abso = labs(real_offset);
            long hour = abso / 3600;
            long minute = (abso % 3600) / 60;

            fprintf(stderr, "%ld:%ld  (%ld)\n", hour, minute, real_offset);

            offset[0] = sign;
            offset[1] = nibble_hex((short) hour / 10);
            offset[2] = nibble_hex((short) hour % 10);
            offset[3] = ':';
            offset[4] = nibble_hex((short) minute / 10);
            offset[5] = nibble_hex((short) minute % 10);
            offset[6] = '\0';
        }

        if(res > 0 && res + offset_size <= size) {
            memcpy(b + res, offset, offset_size);
        }
    }

    // prevent any buffer overflows
    b[size - 1] = '\0';

    return res;
}
