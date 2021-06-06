#include <errno.h>
#include "error.h"

int error_http_status(enum sternenblog_error err) {
    switch(err) {
        case STERNENBLOG_OK:
            return 200;
        case STERNENBLOG_ERROR_NOT_FOUND:
            return 404;
        case STERNENBLOG_ERROR_FORBIDDEN:
            return 403;
        case STERNENBLOG_ERROR_REQUEST:
            return 400;
        case STERNENBLOG_ERROR_SYSTEM:
        case STERNENBLOG_ERROR_CGI:
        case STERNENBLOG_ERROR_UNEXPECTED:
        default:
            return 500;
    }
}

int error_from_errno(void) {
    switch(errno) {
        case EPERM:
        case EACCES:
            return STERNENBLOG_ERROR_FORBIDDEN;
        case ENOENT:
            return STERNENBLOG_ERROR_NOT_FOUND;
        default:
            return STERNENBLOG_ERROR_SYSTEM;
    }
}
