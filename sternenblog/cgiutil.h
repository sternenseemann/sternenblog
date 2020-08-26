/*!
 * @file cgiutil.h
 * @brief Simple CGI/HTTP helper functions used by sternenblog.
 */

#include <stdbool.h>

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

/*!
 * @brief Urlencode a given dynamically allocated string
 *
 * urlencode_realloc() receives a pointer to a pointer to
 * a dynamically allocated string to encode plus its size
 * including the null byte at the end.
 *
 * It then replaces every reserved character in the string
 * except `/` with the appropriate percent encoding. If
 * the size of the buffer is not enough, it uses `realloc()`
 * to increase it.
 *
 * Note that the implementation of url encoding is not 100%
 * correct, but should be good enough in the context of
 * sternenblog. `/` is not encoded since on unix
 * a slash should always a path delimiter and never part of
 * a filename. Another limitation of the url encoding is
 * that it only checks for a list of characters to encode
 * instead of checking if the characters are unreserved
 * and don't need to be encoded which would be more correct.
 * The approach taken has the big advantage that we don't
 * need to worry about UTF-8, which makes the implementation
 * considerably simpler. As a consequence however it will
 * be not aggressive enough in terms of encoding in some
 * cases.
 *
 * On error -1 is returned. In such a case the original
 * pointer remains intact, so you can either `free()` it
 * or continue with the unencoded string.
 *
 * Otherwise it returns new size of the buffer.
 *
 * @param **input pointer to input string
 * @param size size of input string including null byte
 * @return -1 on error, else size of buffer
 */
int urlencode_realloc(char **input, int size);

/*!
 * @brief Returns URL of server addressed by the current CGI request
 *
 * server_url() uses the CGI 1.1 environment variables `SERVER_NAME`
 * and `SERVER_PORT` to construct an URL to the server the current
 * request is addressed to. Since CGI only reveals the HTTP version
 * used and not wether an encrypted version of HTTP is used,
 * server_url() will use the parameter `https` to decide which protocol
 * identifier to prefix.
 *
 * The returned `char *` is dynamically allocated and must be cleaned
 * up using `free()` before it goes out of scope.
 *
 * @param https if true, prefix `https://` else `http://`
 * @return Pointer to dynamically allocated char buffer containing the URL.
 */
char *server_url(bool https);
