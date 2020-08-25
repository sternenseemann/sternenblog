/*!
 * @file template.h
 * @brief Declarations of functions to be implemented by a sternenblog template
 *
 * Requires prior inclusion of core.h.
 *
 * The functions declared in template.h are called by `blog_index()`
 * and `blog_entry()` to generate the HTML document CGI responses
 * involving HTML (contrary to the RSS feed which is independent
 * from templates).
 *
 * These functions can be implemented by a custom C source file
 * in order to customize the HTML output of sternenblog. Every
 * function is expected to output HTML to `stdout`. They themselves
 * can expect to be called in the following order:
 *
 * * template_header()
 * * One of template_single_entry(), template_index_entry (any number
 *   of times) or template_error()
 * * template_footer()
 */

/*!
 * @brief (Meta) data about the page being served
 *
 * `struct template_data` is used to pass information about
 * the current page to the template. It is received as the
 * single argument by all template functions.
 *
 * The following assumptions about its contents can be made:
 *
 * * `page_type == PAGE_TYPE_ENTRY` → `entry != NULL`
 * * `page_type == PAGE_TYPE_ERROR` ⟷ `status != 200`
 * * `page_type != PAGE_TYPE_ERROR` → `script_name != NULL && path_info != NULL`
 * * `page_type == PAGE_TYPE_ERROR` → `entry == NULL`
 */
struct template_data {
  enum page_type page_type;       //!< type of page to render
  int status;                     //!< HTTP status of the response
  struct entry *entry;            //!< Pointer to entry if applicable, else `NULL`
  char *script_name;              //!< value of `SCRIPT_NAME` environment variable
  char *path_info;                //!< value of `PATH_INFO` environment variable
};

/*!
 * @brief Prints beginning of HTML source
 *
 * template_header() is expected to print out the common beginning of
 * any response and allocate any resources the template uses (it's
 * the best place for such things since it is always called as the
 * first template function).
 *
 * Typically it will print the HTML `<head>` and the header part
 * of the `<body>` element which is common for all pages. It may
 * adjust some parts of it (like headings, title, navigations, …)
 * depending on the `data` that is passed.
 *
 * If `data.page_type == PAGE_TYPE_INDEX`, `data.entry` will point
 * to the first entry or be `NULL` if there are no entries.
 *
 * @see struct template_data
 */
void template_header(struct template_data data);

/*!
 * @brief Prints end of HTML source
 *
 * template_footer() should print the common bottom part of any HTML
 * response and free all allocated resources (as it's called last).
 *
 * Usually this involves printing a footer part of the web page and
 * closing the `<body>` and `<html>` elements.
 *
 * If `data.page_type == PAGE_TYPE_INDEX`, `data.entry` will point
 * to the last entry or be `NULL` if there are no entries.
 */
void template_footer(struct template_data data);

/*!
 * @brief Prints HTML snippet for the main part of the page
 *
 * template_main() should print the main part of the HTML source
 * which is located between template_header() and template_footer().
 *
 * Depending on `data.page_type` the following applies:
 *
 * * For `PAGE_TYPE_ENTRY` template_main() is called once and
 *   should print the main part of a single entry page.
 * * For `PAGE_TYPE_ERROR` template_main() is called once and
 *   should print the main part of a page informing the user
 *   about an occurred HTTP error (reflecting `data.status`).
 * * For `PAGE_TYPE_INDEX` template_main() is called 0 to n
 *   times where n is the number of total entries. Each time
 *   it's called it should print a HTML snippet which is
 *   suitable as an index entry. Furthermore it should be
 *   valid HTML regardless how many times it has been called
 *   before and will be called afterwards.
 *
 * @see struct template_data
 */
void template_main(struct template_data data);
