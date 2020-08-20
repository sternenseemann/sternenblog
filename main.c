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
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "core.h"
#include "config.h"
#include "cgiutil.h"
#include "entry.h"
#include "index.h"
#include "stringutil.h"
#include "timeutil.h"
#include "template.h"
#include "xml.h"

// TODO sandbox
//      caching
//      reduce memory usage by only using get_text in the inner loop
//      and unmapping the file directly afterwards

/*!
 * @brief Outputs the CGI response for the index page of the blog
 *
 * This function is called if `PATH_INFO` is `/` or empty.
 *
 * @see template_index_entry
 * @see make_index
 */
void blog_index(char script_name[]);

/*!
 * @brief Outputs the CGI response for a single entry
 *
 * Called if no other special routes are found (index, RSS feed).
 * If `make_entry()` succeeds, the page for the entry is displayed
 * using `template_single_entry()`. If an error occurs,
 * `template_error()` is used.
 *
 * @see make_entry
 * @see template_single_entry
 * @see template_error
 */
void blog_entry(char script_name[], char path_info[]);

/*!
 * @brief Outputs the CGI response for the blog's RSS feed
 *
 * This function is called if `PATH_INFO` is `/rss.xml`.
 *
 * @see make_index
 */
void blog_rss(char script_name[]);

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

    if(script_name == NULL) {
        fputs("Missing SCRIPT_NAME", stderr);

        send_header("Content-type", "text/html");
        send_header("Status", "500 Internal Server Error");
        terminate_headers();

        template_header();
        template_error(500);
        template_footer();

        // TODO exit failure on error?
        return EXIT_SUCCESS;
    }

    if(path_info == NULL || path_info[0] == '\0' || strcmp(path_info, "/") == 0) {
        blog_index(script_name);
    } else if(strcmp(path_info, "/rss.xml") == 0) {
        blog_rss(script_name);
    } else if(strcmp(path_info, "/atom.xml") == 0) {
        // TODO
        return EXIT_FAILURE;
    } else {
        blog_entry(script_name, path_info);
    }

    return EXIT_SUCCESS;
}

void blog_index(char script_name[]) {
    struct entry *entries = NULL;

    int count = make_index(BLOG_DIR, script_name, 0, &entries);

    send_header("Content-type", "text/html");

    if(count < 0) {
        send_header("Status", "500 Internal Server Error");
        terminate_headers();

        template_header();
        template_error(500);
        template_footer();
    } else {
        send_header("Status", "200 OK");
        terminate_headers();

        template_header();

        for(int i = 0; i < count; i++) {
            if(entry_get_text(&entries[i]) != -1) {
                template_index_entry(entries[i]);

                // unmap file
                if(munmap(entries[i].text, entries[i].text_size) != -1) {
                    entries[i].text_size = -1;
                    entries[i].text = NULL;
                }
            }
        }

        template_footer();

        free_index(&entries, count);
    }
}

void blog_entry(char script_name[], char path_info[]) {
    send_header("Content-type", "text/html");

    struct entry entry;
    int status = make_entry(BLOG_DIR, script_name, path_info, &entry);

    if(status == 200 && entry_get_text(&entry) == -1) {
        status = 500;
    }

    if(status != 200) {
        send_header("Status", http_status_line(status));
        terminate_headers();

        template_header();
        template_error(status);
        template_footer();
    } else {
        send_header("Status", "200 OK");
        terminate_headers();

        template_header();
        template_single_entry(entry);
        template_footer();
    }

    free_entry(entry);
}

void blog_rss(char script_name[]) {
    // index
    struct entry *entries = NULL;

    int count = make_index(BLOG_DIR, script_name, 1, &entries);

    if(count < 0) {
        send_header("Status", "500 Internal Server Error");
        send_header("Content-type", "text/plain");
        terminate_headers();

        puts("Internal Server Error");
        return;
    }

    send_header("Status", "200 OK");
    send_header("Content-type", "application/rss+xml");
    terminate_headers();

    struct xml_context ctx;
    new_xml_context(&ctx);

    ctx.closing_slash = 1;
    ctx.warn = stderr; // dev

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

    xml_open_tag(&ctx, "link");
    xml_escaped(&ctx, BLOG_SERVER_URL);
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

    char ttl_string[32];
    if(snprintf(ttl_string, sizeof ttl_string, "%d", BLOG_RSS_TTL) >= 0) {
        xml_open_tag(&ctx, "ttl");
        xml_escaped(&ctx, ttl_string);
        xml_close_tag(&ctx, "ttl");
    }

    char *rss_link = catn_alloc(3, BLOG_SERVER_URL, script_name, "/rss.xml");
    if(rss_link != NULL) {
        xml_empty_tag(&ctx, "atom:link", 3,
                      "rel", "self",
                      "href", rss_link,
                      "type", "application/rss+xml");
        free(rss_link);
    }

    for(int i = 0; i < count; i++) {
        xml_open_tag(&ctx, "item");
        xml_open_tag(&ctx, "title");
        xml_escaped(&ctx, entries[i].title);
        xml_close_tag(&ctx, "title");

        xml_open_tag(&ctx, "link");
        xml_escaped(&ctx, BLOG_SERVER_URL);
        xml_escaped(&ctx, entries[i].link);
        xml_close_tag(&ctx, "link");

        xml_open_tag(&ctx, "guid");
        xml_escaped(&ctx, BLOG_SERVER_URL);
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
    }

    xml_close_all(&ctx);

    free_index(&entries, count);

    del_xml_context(&ctx);
}
