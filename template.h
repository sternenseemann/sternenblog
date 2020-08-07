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
 * @brief Prints beginning of HTML source
 *
 * template_header() is expected to print out the common beginning of
 * any response and allocate any resources the template uses (it's
 * the best place for such things since it is always called as the
 * first template function).
 *
 * Typically it will print the HTML `<head>` and the header part
 * of the `<body>` element which is common for all pages.
 *
 * If you are using `xml.h`, this is a good place to call new_xml_context().
 */
void template_header(void);

/*!
 * @brief Prints end of HTML source
 *
 * template_footer() should print the common bottom part of any HTML
 * response and free all allocated resources (as it's called last).
 *
 * Usually this involves printing a footer part of the web page and
 * closing the `<body>` and `<html>` elements.
 *
 * If you are using `xml.h`, this is a good place to call del_xml_context().
 */
void template_footer(void);

/*!
 * @brief Prints HTML snippet for a single entry on the index page
 *
 * template_index_entry() is called for every entry that is part of
 * the index page. It should print out the repeating content structure
 * filled with the respective values for each entry.
 *
 * It gets passed a fully constructed `struct entry` with its `text`
 * field populated, i. e. entry_get_text() has been called
 *
 * This function is essentially like template_single_entry(), but:
 *
 * * It must make sure to output appropriate and correct HTML
 *   if called repeatedly
 * * Should reflect that it's an index page, i. e. link to
 *   the entries' individual pages and maybe display less
 *   detail than single pages.
 *
 * @see template_index_entry
 * @see struct entry
 * @see entry_get_text
 */
void template_index_entry(struct entry entry);

/*!
 * @brief Prints HTML snippet for an entry's own page
 *
 * template_single_entry() is like template_index_entry()
 * and receives a fully populated `struct entry` as well,
 * but it's a single page of an entry, so it may display
 * more details and link back to the index.
 *
 * @see struct entry
 * @see entry_get_text
 */
void template_single_entry(struct entry entry);

/*!
 * @brief Prints HTML snippet for an error page's main entry
 *
 * This should print main part of the page informing the user
 * about an error. It may display different messages depending
 * on the type of error.
 *
 * @param code HTTP status code of the error
 */
void template_error(int code);
