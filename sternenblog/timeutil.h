/*!
 * @file
 * @brief Utilities for rendering timestamps as strings
 */
enum time_format {
    RSS_TIME_FORMAT,          //!< RFC822 formatted time with 4 instead of 2 year digits
    ATOM_TIME_FORMAT,         //!< RFC3339 formatted time
    HTML_TIME_FORMAT_READABLE //!< like `ATOM_TIME_FORMAT`, but with space between date and time
};

/*!
 * @brief Maximum size necessary to contain the output of flocaltime()
 */
#define MAX_TIMESTR_SIZE 32
// max HTML/Atom: 24 + NUL byte
// max RSS:       31 + NUL byte

/*!
 * @brief Format given timestamp as a string in the local timezone
 *
 * flocaltime() is a wrapper around `strftime()` which supports
 * a specific set of output formats. In contrast to `strftime()`
 * it can output correct RFC3339 time strings and does localtime
 * resolution for you.
 *
 * Example usage to print a RFC3339 formatted timestamp:
 *
 * ```
 * time_t some_time;
 * char strtime[MAX_TIMESTR_SIZE];
 *
 * if(flocaltime(strtime, ATOM_TIME_FORMAT, MAX_TIMESTR_SIZE, &some_time) > 0) {
 *   puts(strtime);
 * }
 * ```
 *
 * @param b output buffer
 * @param type time format to use for output
 * @param size number of `char`s the buffer can hold
 * @param time pointer to timestamp
 * @return `0` on error, otherwise length of the string placed in `b` excluding terminating `NUL` byte
 */
size_t flocaltime(char *b, enum time_format type, size_t size, const time_t *time);
