/*!
 * @file cgiutil.h
 * @brief Simple CGI/HTTP helper functions used by sternenblog.
 */

/*!
 * @brief Print a HTTP header
 *
 * Prints a HTTP Header to `stdout` like CGI requires.
 *
 * @param key Name of the HTTP Header
 * @param val Contents of the header to send
 */
void send_header(char key[], char val[]);

/*!
 * @brief Print end of HTTP header section
 *
 * Terminates the header section of a CGI/HTTP Response by printing `\r\n` to `stdout`.
 */
void terminate_headers(void);

/*!
 * @brief Value of a HTTP status header for a given status code.
 *
 * Helper function that returns the status code plus its
 * accompanying reason phrase as a string.
 *
 * The value is statically allocated so do not attempt
 * to free it.
 *
 * Example usage:
 *
 * ```
 * send_header("Status", http_status_line(404);
 * // Prints: Status: 404 Not Found
 * ```
 *
 * @param status HTTP status code
 * @return status code and reason phrase as a string.
 */
char *http_status_line(int status);

/*!
 * @brief Return HTTP error code for given errno
 *
 * Incomplete mapping of `errno`s to HTTP error codes.
 * Defaults to 500.
 *
 * @param err POSIX errno
 * @return HTTP error code
 */
int http_errno(int err);
