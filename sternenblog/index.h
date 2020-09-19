/*!
 * @file index.h
 * @brief Construction and destruction of entry indices
 */

#ifndef STERNENBLOG_INDEX_H
#define STERNENBLOG_INDEX_H

#include "core.h"
#include <stdbool.h>

/*!
 * @brief Build index of given `blog_dir`
 *
 * Allocates an array of entries, reads `blog_dir` and adds a `struct entry` to the
 * array for every file for which `make_entry()` reports no error. It doesn't enter
 * subdirectories.
 *
 * Note that it's error handling is very simple and it doesn't distinguish between an
 * error occuring and the end of the directory.
 *
 * @param blog_dir path to the directory entries are stored in
 * @param script_name the value of the `SCRIPT_NAME` environment variable
 * @param get_text whether to call `entry_get_text()` on successfully constructed entries
 * @param entries pointer to an array that should be used
 * @return size of the dynamically allocated entries array
 * @see free_index
 */
int make_index(const char *blog_dir, char *script_name, bool get_text, struct entry *entries[]);

/*!
 * @brief Free dynamically allocated index
 *
 * Call `free_entry()` for every entry and free entire array afterwards.
 *
 * @param entries pointer to array of entries
 * @param count size of the given array
 */
void free_index(struct entry *entries[], int count);

#endif
