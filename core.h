/*!
 * @file core.h
 * @brief Central type definitions of sternenblog
 */
#include <time.h>

/*!
 * @brief Resolved blog entry
 *
 * Represents a resolved entry and should only be
 * constructed using `make_entry()` and populated using
 * `entry_get_text()`.
 *
 * If constructed correctly, you can expect such an entry to exist.
 *
 * Use `free_entry()` to free allocated `char *` in an `entry`
 * constructed by `make_entry()` (and `entry_get_text()`).
 *
 * @see make_entry
 * @see entry_get_text
 * @see free_entry
 * @see make_index
 */
struct entry {
    // mandatory: part of each well-formed entry
    time_t time;       //!< last modification time of the entry
    char *path;        //!< path (on disk) to the entry
    char *link;        //!< absolute path on the http server to the entry
    char *title;       //!< title of the post, currently `PATH_INFO` without the initial slash
    size_t text_size;  //!< size of text, -1 to indicate it's missing
    // optional: may be NULL, depending on context
    char *text;        //!< contents of the entry (mmap-ed file) or `NULL`
};

