#ifndef STERNENBLOG_ERROR_H
#define STERNENBLOG_ERROR_H

enum sternenblog_error {
  //! No error
  STERNENBLOG_OK = 0,
  //! libc call returned error, check `errno` for details
  STERNENBLOG_ERROR_SYSTEM = -1,
  //! requested item doesn't exist
  STERNENBLOG_ERROR_NOT_FOUND = -2,
  //! access to the requested item isn't allowed (either by system or us)
  STERNENBLOG_ERROR_FORBIDDEN = -3,
  //! CGI environment is not correctly set up
  STERNENBLOG_ERROR_CGI = -4,
  //! User send a request that doesn't make sense to us
  STERNENBLOG_ERROR_REQUEST = -5,
  //! The unexpected happened like an assumption not holding.
  //! If this error is generated it is likely a bug
  STERNENBLOG_ERROR_UNEXPECTED = -6,
};

int error_http_status(enum sternenblog_error err);

int error_from_errno(void);

#endif
