/*!
 * @file main.c
 * @brief Source of the `sternenblog.cgi` executable
 */

/*!
 * @mainpage sternenblog
 *
 * sternenblog is a file based blog software written in C for CGI.
 *
 * @section user_doc User documentation
 *
 * For user documentation and information on
 * configuring `sternenblog` refer to
 * `man sternenblog.cgi(1)` and config.example.h.
 *
 * @section dev_doc Developer documentation
 *
 * @subsection tpl_doc Templating
 *
 * A template is a single C file implementing all the
 * functions declared in template.h. The documentation
 * for the header file also includes detailed explanation
 * of what every function must do. For inspiration, read through the
 * @subpage example_template "default template", `templates/simple.c`.
 *
 * To change the template sternenblog is using set `TEMPLATE`
 * in `config.mk` to the absolute or relative path to your
 * template's C file with the `.c` extension omitted.
 *
 * @subsection xml_doc XML Output Library
 *
 * sternenblog includes a small library for outputting XML
 * which is useful in an CGI environment. You can use this
 * library in your template using the following include:
 *
 * ```
 * #include <xml.h>
 * ```
 *
 * Its usage and features are documented in xml.h.
 *
 * sternenblog uses xml.h internally for generating its
 * rss feed and its default template.
 *
 * @subsection cgi_doc CGI Helpers
 *
 * cgiutil.h includes a few simple helpers for CGI.
 *
 * @subsection int_doc Internals
 *
 * core.h defines the central type of sternenblog, `struct entry`.
 * Entries is essentially all sternenblog knows and handles.
 * It either builds an index (listing) of them using `make_index()`
 * from index.h or gets a single one using `make_entry()` from entry.h
 * and serves the result to the user.
 *
 * This happens either via a RSS feed generated using `blog_rss()`
 * or as html pages which are generated by `blog_index()` and
 * `blog_entry()` calling the template functions defined in template.h.
 *
 * For a more detailed explanation than this overview read the documentation
 * of main.c and the header files mentioned here. The source code should
 * also be a good source of insight if you want to dig deeper.
 *
 * @page example_template Example template
 *
 * This is the default template sternenblog uses. It is pretty
 * simple and can serve as a good reference point. It utilizes
 * sternenblog-specific exposed functions from xml.h and
 * `http_status_line()` from cgiutil.h.
 *
 * @include simple.c
 *
 */
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

#include "sternenblog/core.h"
#include "sternenblog/cgiutil.h"
#include "sternenblog/entry.h"
#include "sternenblog/index.h"
#include "sternenblog/stringutil.h"
#include "sternenblog/timeutil.h"
#include "sternenblog/template.h"
#include "sternenblog/xml.h"

/*!
 * @brief Routing enum to differentiate feeds
 *
 * Used in routing to differentiate between
 *
 * * Feeds (RSS vs. Atom)
 * * Feeds and non-feeds (feeds are a special type of `PAGE_TYPE_INDEX`)
 */
enum feed_type {
    FEED_TYPE_NONE,
    FEED_TYPE_RSS,
    FEED_TYPE_ATOM
};

/*!
 * @brief Send terminated default header compound
 *
 * Sends `Status` and `Content-type` headers for the given
 * values and a `Cache-Control` header if applicable before
 * calling `terminate_headers()`.
 */
void send_standard_headers(int status, char content_type[]);

/*!
 * @brief Outputs the CGI response for the blog's RSS feed
 *
 * This function is called if `PATH_INFO` is `/rss.xml`.
 *
 * @see make_index
 * @see blog_atom
 */
void blog_rss(char script_name[], struct entry *entries, int count);

/*!
 * @brief Outputs the CGI response for the blog's Atom feed
 *
 * This function is called if `PATH_INFO` is `/atom.xml`.
 *
 * @see make_index
 * @see blog_rss
 */
void blog_atom(char script_name[], struct entry *entries, int count);

/*!
 * @brief Implements routing of requests
 *
 * @see blog_index
 * @see blog_rss
 * @see blog_entry
 */
int main(void) {
    char *path_info = getenv("PATH_INFO");
    char *script_name = getenv("SCRIPT_NAME");

    enum page_type page_type;
    enum feed_type is_feed = FEED_TYPE_NONE;

    struct entry *entries = NULL;
    int count = 0;
    int status = 500;

    // Routing: determine page_type and feed_type
    // already allocate data for single entries
    if(script_name == NULL) {
        fputs("Missing CGI environment variable SCRIPT_NAME\n", stderr);
        page_type = PAGE_TYPE_ERROR;
    } else if(path_info == NULL || path_info[0] == '\0' || strcmp(path_info, "/") == 0) {
        // make sure clean URLs are generated
        setenv("PATH_INFO", "", 1);

        page_type = PAGE_TYPE_INDEX;
    } else if(strcmp(path_info, "/rss.xml") == 0) {
        is_feed = FEED_TYPE_RSS;
        page_type = PAGE_TYPE_INDEX;
    } else if(strcmp(path_info, "/atom.xml") == 0) {
        page_type = PAGE_TYPE_INDEX;
        is_feed = FEED_TYPE_ATOM;
    } else {
        // single entry is just a special index
        entries = malloc(sizeof(struct entry));
        if(entries == NULL) {
            status = 500;
        } else {
            status = make_entry(BLOG_DIR, script_name, path_info, entries);
        }

        if(status == 200 && entry_get_text(entries) != -1) {
            page_type = PAGE_TYPE_ENTRY;
            count = 1;
        } else {
            page_type = PAGE_TYPE_ERROR;
        }
    }

    // confirm index is allocated if we are serving a feed
    assert(is_feed == FEED_TYPE_NONE || page_type == PAGE_TYPE_INDEX);

    // construct index for feeds and index page
    if(page_type == PAGE_TYPE_INDEX) {
        count = make_index(BLOG_DIR, script_name, 0, &entries);

        if(count < 0) {
            page_type = PAGE_TYPE_ERROR;
            status = 500;
        } else {
            page_type = PAGE_TYPE_INDEX;
            status = 200;
        }
    }

    // confirm status and page_type match
    assert(status == 200 || page_type == PAGE_TYPE_ERROR);
    assert(page_type != PAGE_TYPE_ERROR || status != 200);

    // initial contents of data, changed in loop for PAGE_TYPE_INDEX
    struct template_data data;
    data.page_type = page_type;
    data.status = status;
    data.script_name = script_name;
    data.path_info = path_info;

    // confirm that we have SCRIPT_NAME and PATH_INFO unless an error occurred
    assert(data.page_type != PAGE_TYPE_ERROR ||
           (data.path_info != NULL && data.script_name != NULL));

    // make sure that PAGE_TYPE_ENTRY will have an entry set in template_header
    if(count > 0) {
        data.entry = entries;
    } else {
        data.entry = NULL;
    }
    assert(page_type != PAGE_TYPE_ENTRY || data.entry != NULL);

    // render response
    if(page_type == PAGE_TYPE_ERROR) {
        send_standard_headers(status, "text/html");

        template_header(data);
        template_main(data);
        template_footer(data);
    } else if(is_feed == FEED_TYPE_NONE) {
        // either PAGE_TYPE_INDEX or PAGE_TYPE_ENTRY
        send_standard_headers(200, "text/html");

        template_header(data);

        // confirm that PAGE_TYPE_ENTRY → count == 1
        assert(page_type != PAGE_TYPE_ENTRY || count == 1);

        for(int i = 0; i < count; i++) {
            if(entries[i].text != NULL || entry_get_text(&entries[i]) != -1) {
                data.entry = &entries[i];
                assert(data.entry != NULL);

                template_main(data);

                entry_unget_text(&entries[i]);
            }
        }

        template_footer(data);
    } else if(is_feed == FEED_TYPE_RSS) {
        blog_rss(script_name, entries, count);
    } else if(is_feed == FEED_TYPE_ATOM) {
        blog_atom(script_name, entries, count);
    }

    // clean up
    if(entries != NULL) {
        free_index(&entries, count);
    }

    return EXIT_SUCCESS;
}

void send_standard_headers(int status, char content_type[]) {
    send_header("Status", http_status_line(status));
    send_header("Content-type", content_type);

#ifdef BLOG_CACHE_MAX_AGE
    // TODO correct sized buffer, no snprintf
    char max_age[256];
    int result = snprintf(max_age, sizeof max_age, "max-age=%d", BLOG_CACHE_MAX_AGE);

    // make sure there won't be a buffer overrun
    max_age[sizeof max_age - 1] = '\0';

    if(result > 0) {
        send_header("Cache-Control", max_age);
    }
#endif

    terminate_headers();
}

void blog_rss(char script_name[], struct entry *entries, int count) {
    send_standard_headers(200, "application/rss+xml");

    struct xml_context ctx;
    new_xml_context(&ctx);

    xml_raw(&ctx, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");
    xml_open_tag_attrs(&ctx, "rss", 2, "version", "2.0", "xmlns:atom", "http://www.w3.org/2005/Atom");
    xml_open_tag(&ctx, "channel");

    xml_open_tag(&ctx, "title");
    xml_escaped(&ctx, BLOG_TITLE);
    xml_close_tag(&ctx, "title");

    xml_open_tag(&ctx, "description");
    xml_open_cdata(&ctx);
    xml_raw(&ctx, BLOG_DESCRIPTION);
    xml_close_cdata(&ctx);
    xml_close_tag(&ctx, "description");

    char *external_url = server_url(BLOG_USE_HTTPS);

    xml_open_tag(&ctx, "link");
    if(external_url != NULL) {
        xml_escaped(&ctx, external_url);
    }
    xml_escaped(&ctx, script_name);
    xml_escaped(&ctx, "/");
    xml_close_tag(&ctx, "link");

    if(count > 0) {
        time_t update_time = entries[0].time;
        char strtime_update[MAX_TIMESTR_SIZE];

        if(flocaltime(strtime_update, RSS_TIME_FORMAT, MAX_TIMESTR_SIZE, &update_time) > 0) {
            xml_open_tag(&ctx, "lastBuildDate");
            xml_escaped(&ctx, strtime_update);
            xml_close_tag(&ctx, "lastBuildDate");
        }
    }

    char *rss_link = catn_alloc(3, external_url, script_name, "/rss.xml");
    if(rss_link != NULL) {
        xml_empty_tag(&ctx, "atom:link", 3,
                "rel", "self",
                "href", rss_link,
                "type", "application/rss+xml");
        free(rss_link);
    }

    for(int i = 0; i < count; i++) {
        if(entry_get_text(&entries[i]) != -1) {
            xml_open_tag(&ctx, "item");
            xml_open_tag(&ctx, "title");
            xml_escaped(&ctx, entries[i].title);
            xml_close_tag(&ctx, "title");

            xml_open_tag(&ctx, "link");
            if(external_url != NULL) {
                xml_escaped(&ctx, external_url);
            }
            xml_escaped(&ctx, entries[i].link);
            xml_close_tag(&ctx, "link");

            xml_open_tag(&ctx, "guid");
            if(external_url != NULL) {
                xml_escaped(&ctx, external_url);
            }
            xml_escaped(&ctx, entries[i].link);
            xml_close_tag(&ctx, "guid");

            if(entries[i].text_size > 0) {
                xml_open_tag(&ctx, "description");
                xml_open_cdata(&ctx);
                xml_raw(&ctx, entries[i].text);
                xml_close_cdata(&ctx);
                xml_close_tag(&ctx, "description");
            }

            char strtime_entry[MAX_TIMESTR_SIZE];

            if(flocaltime(strtime_entry, RSS_TIME_FORMAT, MAX_TIMESTR_SIZE, &entries[i].time) > 0) {
                xml_open_tag(&ctx, "pubDate");
                xml_escaped(&ctx, strtime_entry);
                xml_close_tag(&ctx, "pubDate");
            }

            xml_close_tag(&ctx, "item");

            entry_unget_text(&entries[i]);
        }
    }

    xml_close_all(&ctx);

    free(external_url);

    del_xml_context(&ctx);
}

void blog_atom(char script_name[], struct entry *entries, int count) {
    struct xml_context ctx;
    new_xml_context(&ctx);

    char *external_url = server_url(BLOG_USE_HTTPS);
    char *self_url = catn_alloc(3, external_url, script_name, "/atom.xml");
    char *html_url = catn_alloc(3, external_url, script_name, "/");

    send_standard_headers(200, "application/atom+xml");

    xml_raw(&ctx, "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    xml_open_tag_attrs(&ctx, "feed", 1, "xmlns", "http://www.w3.org/2005/Atom");

    xml_open_tag(&ctx, "title");
    xml_escaped(&ctx, BLOG_TITLE);
    xml_close_tag(&ctx, "title");

    if(self_url != NULL) {
        xml_open_tag(&ctx, "id");
        xml_escaped(&ctx, self_url);
        xml_close_tag(&ctx, "id");

        xml_empty_tag(&ctx, "link", 2, "rel", "self", "href", self_url);
        free(self_url);
    }

    if(html_url != NULL) {
        xml_empty_tag(&ctx, "link", 3, "rel", "alternate", "type", "text/html", "href", html_url);
        // freed after author element
    }

    xml_open_tag(&ctx, "author");
    xml_open_tag(&ctx, "name");
#ifdef BLOG_AUTHOR
    xml_escaped(&ctx, BLOG_AUTHOR);
#else
    struct passwd *user;
    uid_t uid = geteuid();
    user = getpwuid(uid);
    xml_escaped(&ctx, user->pw_name);
#endif
    xml_close_tag(&ctx, "name");

    if(html_url != NULL) {
        xml_open_tag(&ctx, "uri");
        xml_escaped(&ctx, html_url);
        xml_close_tag(&ctx, "uri");
        free(html_url);
    }

    xml_close_tag(&ctx, "author");

    if(count > 0) {
        time_t update_time = entries[0].time;
        char strtime_update[MAX_TIMESTR_SIZE];
        if(flocaltime(strtime_update, ATOM_TIME_FORMAT, MAX_TIMESTR_SIZE, &update_time) > 0) {
            xml_open_tag(&ctx, "updated");
            xml_escaped(&ctx, strtime_update);
            xml_close_tag(&ctx, "updated");
        }
    }

    for(int i = 0; i < count; i++) {
        if(entry_get_text(&entries[i]) != -1) {
            xml_open_tag(&ctx, "entry");

            xml_open_tag(&ctx, "id");
            if(external_url != NULL) {
                xml_escaped(&ctx, external_url);
            }
            xml_escaped(&ctx, entries[i].link);
            xml_close_tag(&ctx, "id");

            xml_open_tag(&ctx, "title");
            xml_escaped(&ctx, entries[i].title);
            xml_close_tag(&ctx, "title");

            char strtime_entry[MAX_TIMESTR_SIZE];
            if(flocaltime(strtime_entry, ATOM_TIME_FORMAT, MAX_TIMESTR_SIZE, &entries[i].time) > 0) {
                xml_open_tag(&ctx, "updated");
                xml_escaped(&ctx, strtime_entry);
                xml_close_tag(&ctx, "updated");
            }

            char *entry_url = catn_alloc(2, external_url, entries[i].link);
            if(entry_url != NULL) {
                xml_empty_tag(&ctx, "link", 3, "rel", "alternate", "type", "text/html", "href", entry_url);
                free(entry_url);
            }

            xml_open_tag_attrs(&ctx, "content", 1, "type", "html");
            xml_open_cdata(&ctx);
            xml_raw(&ctx, entries[i].text);
            xml_close_cdata(&ctx);
            xml_close_tag(&ctx, "content");

            xml_close_tag(&ctx, "entry");

            entry_unget_text(&entries[i]);
        }
    }

    xml_close_tag(&ctx, "feed");

    free(external_url);

    del_xml_context(&ctx);
}
