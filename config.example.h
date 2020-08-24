/*!
 * @file config.example.h
 * @brief Example configuration header of sternenblog
 */

/*!
 * @name Operational Settings
 * @{
 */

/*!
 * @brief Directory entries are stored in
 *
 * Directory blog entries are stored in.
 * Must either be an absolute path or start with "./".
 * Dotfiles and subdirectories will be ignored.
 */
#define BLOG_DIR "/srv/sternenblog/"

/*!
 * @brief Public URL of the webserver
 *
 * (External) URL the server that the script is running on is reachable via.
 * This should be the base URL of your webserver without any trailing slashes
 * or subpaths, as sternenblog will use SCRIPT_NAME and PATH_INFO to figure
 * out the path from the root of the server.
 */
#define BLOG_SERVER_URL "http://localhost"

/*!
 * @brief Enable / Disable strict access check
 *
 * If enabled, sternenblog will only serve files which are either owned
 * by the user or group it is running as. This usually means that in order
 * to be served files must be `chown`ed to the group or user the webserver
 * is running as. Consequently it is harder to accidentally make files public.
 *
 * If disabled, sternenblog will serve any file in `BLOG_DIR` it can read.
 *
 * @see BLOG_DIR
 */
#define BLOG_STRICT_ACCESS 1

//! @}

/*!
 * @name Site Metadata
 * @{
 */

/*!
 * @brief Site title
 *
 * Title of the site to serve.
 * Will be used for the feeds and in templates to, e. g. set the HTML title tag.
 */
#define BLOG_TITLE "sternenblog"

/*!
 * @brief Site's Author
 *
 * Name of the author of the site served.
 * Used to set the feed's author in the atom feed,
 * ignored for RSS (since it is not recommended to
 * use for single author feeds there which is the only type
 * sternenblog can handle).
 *
 * Optional setting, if missing replaced by the username
 * sternenblog is running as.
 */
#define BLOG_AUTHOR "Jane Doe"

/*!
 * @brief Site description
 *
 * Description of the site to serve.
 * Will be used for the feeds and may be utilized by templates.
 *
 * @see https://cyber.harvard.edu/rss/rss.html#requiredChannelElements
 */
#define BLOG_DESCRIPTION "my personal blog"

//! @}

/*!
 * @name Caching
 * @{
 */

/*!
 * @brief Time a browsers should cache a response of sternenblog
 *
 * Time in seconds that should be the value of the `max-age` field
 * of the `Cache-Control` header. It determines the time in seconds
 * a response of sternenblog should be considered “fresh”.
 *
 * During this time browsers will used the cached version of the
 * response (if present) for a given URL. This is useful to reduce
 * server load, since browsers will request resources less often
 * which means they have to be generated less often as well
 * (especially the index).
 *
 * The value should be at least the time you expect users to spend
 * on your website although you might want to go lower if your
 * website updates _very_ frequently.
 *
 * The header will also be set for the feeds, but the RSS
 * `ttl` element won't be generated based on this value.
 *
 * Optional setting.
 *
 * Requires HTTP/1.1, but sternenblog won't check the used protocol version
 * before sending this header.
 *
 * @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control
 */
#define BLOG_CACHE_MAX_AGE 3600

//! @}

/*!
 * @name Template settings
 * @{
 */

/*!
 * @brief CSS url to use it `template/simple.c`
 *
 * CSS Stylesheet to use for the entire site. The default template will use
 * the Stylesheet URL given here if present.
 *
 * Optional setting.
 */
#define BLOG_CSS "/sternenblog.css"
//! @}
