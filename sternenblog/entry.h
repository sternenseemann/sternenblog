/*!
 * @file entry.h
 * @brief Construction and destruction of entries
 *
 * Requires prior inclusion of core.h.
 */

/*!
 * @brief Construct an entry for a given `PATH_INFO`
 *
 * `make_entry()` first does a sanity check of the provided `path_info`:
 *
 * * It must start with a slash
 * * It may not contain any path segments which start with a dot
 *   (i. e. no dotfiles and no `.` and `..`)
 * * It may not contain any double slashes
 *
 * `make_entry()` currently has no support for any kind of escaped `PATH_INFO`.
 * This might need some work in the future, but seems unnecessary at the moment
 * RFC3875 doesn't specify any escaping mechanism for `PATH_INFO` and leaves it
 * to the webserver to deal with cases like `foo%2dbar` which is indistinguishable
 * from `foo/bar` in its decoded form.
 *
 * Note that accessing subdirectories is possible and allowed, i. e. a `path_info` of
 * `"/foo/bar"` will result in an entry being constructed for `<blog_dir>/foo/bar`.
 * This behavior is sometimes useful (entries in subdirectories are not included in
 * any indices), but may also be confusing. In the future an option to disable this
 * may be added.
 *
 * Before constructing the entry, `make_entry()` calls `stat()` to check if the given
 * entry a) exists b) is a regular file and c) is owned by the current processes user
 * or group. The last check ensures that the file is not only readable for the webserver,
 * but also owned by either its group or its user. This lessens the likelyhood of
 * something accidentially being processed by `make_entry()`, since usually a
 * `chown http:http` or similar will be necessary to satisfy the check.
 *
 * After that the `entry` structure is populated:
 *
 * * `path` is set to the constructed path to the entry file (dynamically allocated)
 * * `title` is set to `path_info` with the leading slash removed (dynamically allocated)
 * * `time` is set to the file's modification time
 * * `link` is set to `script_name` and `path_info` concatenated which is the absolute web
 *   server path corresponding to the entry
 * * `text_size` is set to `-1`
 * * `text` is set to `NULL`
 *
 * `make_entry()` may fail at any point with parts of the struct already containing
 * pointers to dynamically allocated memory. It is always safe to call `free_entry()`
 * after calling `make_entry()`, so you should make sure to do just that in case of
 * both error and success.
 *
 * @param blog_dir Directory blog entries are stored in, usually `BLOG_DIR`
 * @param path_info `PATH_INFO` CGI environment variable
 * @param script_name `SCRIPT_NAME` CGI environment variable
 * @param entry Uninitialized entry structure to update
 *
 * @return 200 on success, an appropriate HTTP status code on error
 *
 * @see struct entry
 * @see entry_get_text
 * @see free_entry
 * @see make_index
 */
int make_entry(const char *blog_dir, char *script_name, char *path_info, struct entry *entry);

/*!
 * @brief Populate an `entry`'s `text` field
 *
 * Reads the contents of `entry->path` into memory using `mmap()` and sets
 * `entry->text` and `entry->text_size` accordingly.
 *
 * Must be called on an already completely constructed entry.
 *
 * @return 0 on success, -1 on error, currently errno is not set correctly
 *
 * @see entry_unget_text
 * @see make_entry
 * @see free_entry
 */
int entry_get_text(struct entry *entry);

/*!
 * @brief Unmap the file referenced in a `struct entry`
 *
 * Tries to `munmap()` the file pointed to by `entry->text`
 * if present, and updates `entry->text_size` accordingly.
 *
 * The rest of the struct is left untouched.
 *
 * @see free_entry
 */
void entry_unget_text(struct entry *entry);

/*!
 * @brief Free dynamically allocated parts on an `entry`
 *
 * Frees any non `NULL` pointers in the given `entry` structure.
 * `make_entry()` initializes all pointers as `NULL` first thing
 * after being called, so you can always call `free_entry()` after
 * `make_entry()`.
 *
 * It also unmaps the mapped file in `text` if it is not `NULL`
 * using `entry_unget_text()`.
 *
 * Warning: It won't call `free()` on the entire entry structure.
 *
 * @see entry_unget_text
 */
void free_entry(struct entry *entry);
