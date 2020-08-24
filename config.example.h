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
 * @brief TTL of RSS feed
 *
 * TTL (time to live) of the RSS feed in minutes. Feed readers may use this
 * value to determine how often to refresh the RSS feed.
 *
 * @see https://cyber.harvard.edu/rss/rss.html#ltttlgtSubelementOfLtchannelgt
 */
#define BLOG_RSS_TTL 60

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
