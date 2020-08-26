/*!
 * @file
 * @brief Utilities for string construction
 */

/*!
 * @brief Returns hex digit for given integer
 *
 * Will return appropriate `char` in range 0-F
 * for input in range 0-15. Can be abused to
 * return decimal digits for range 0-9.
 */
char nibble_hex(short h);

/*!
 * @brief Concatenate arbitrary number of strings into
 * dynamically allocated buffer
 *
 * catn_alloc() concats the `n` given strings into a
 * dynamically allocated and resized buffer and returns
 * it. This buffer must be cleaned up by `free()` before
 * it goes out of scope.
 *
 * @param n number of strings given as `va_args`
 * @return pointer to concatenated strings or `NULL` on error.
 */
char *catn_alloc(size_t n, ...);
