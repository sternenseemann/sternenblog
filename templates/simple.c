#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <core.h>
#include <template.h>
#include <config.h>
#include <cgiutil.h>
#include <stringutil.h>
#include <timeutil.h>
#include <xml.h>

static struct xml_context ctx;

void output_entry_time(struct xml_context *ctx, struct entry entry) {
    char strtime[MAX_TIMESTR_SIZE];

    if(flocaltime(strtime, HTML_TIME_FORMAT_READABLE, MAX_TIMESTR_SIZE, &entry.time) > 0) {
        xml_open_tag_attrs(ctx, "time", 1, "datetime", strtime);
        xml_raw(ctx, strtime);
        xml_close_tag(ctx, "time");
    }
}

void template_header(void) {
    new_xml_context(&ctx);
    ctx.warn = stderr;
    ctx.closing_slash = 0;

    xml_raw(&ctx, "<!doctype html>");
    xml_open_tag_attrs(&ctx, "html", 1, "lang", "en");

    xml_open_tag(&ctx, "head");

    xml_empty_tag(&ctx, "meta", 1, "charset", "utf-8");

   #ifdef BLOG_CSS
    xml_empty_tag(&ctx, "link", 3,
                  "rel", "stylesheet",
                  "type", "text/css",
                  "href", BLOG_CSS);
   #endif

    xml_open_tag(&ctx, "title");
    xml_raw(&ctx, BLOG_TITLE);
    xml_close_tag(&ctx, "title");

    xml_close_tag(&ctx, "head");

    xml_open_tag(&ctx, "body");
    xml_open_tag(&ctx, "header");
    xml_open_tag(&ctx, "h1");
    xml_raw(&ctx, BLOG_TITLE);
    xml_close_including(&ctx, "header");

    xml_open_tag(&ctx, "main");
}

void template_footer(void) {
    xml_close_tag(&ctx, "main");

    xml_open_tag(&ctx, "footer");

    char *script_name = getenv("SCRIPT_NAME");
    char *rss_link = catn_alloc(2, script_name, "/rss.xml");
    char *atom_link = catn_alloc(2, script_name, "/atom.xml");

    if(rss_link != NULL) {
        xml_open_tag_attrs(&ctx, "a", 1, "href", rss_link);
        xml_escaped(&ctx, "RSS Feed");
        xml_close_tag(&ctx, "a");

        free(rss_link);
    }

    if(atom_link != NULL) {
        xml_raw(&ctx, " &bull; ");

        xml_open_tag_attrs(&ctx, "a", 1, "href", atom_link);
        xml_escaped(&ctx, "Atom Feed");
        xml_close_tag(&ctx, "a");

        free(atom_link);
    }

    xml_close_all(&ctx);

    del_xml_context(&ctx);
}

void template_single_entry(struct entry entry) {
    xml_open_tag(&ctx, "article");

    if(entry.text_size > 0) {
        xml_open_tag_attrs(&ctx, "div", 1, "class", "content");
        xml_raw(&ctx, entry.text);
        xml_close_tag(&ctx, "div");
    }

    char *script_name = getenv("SCRIPT_NAME");

    xml_open_tag_attrs(&ctx, "div", 1, "class", "meta");
    xml_open_tag(&ctx, "ul");

    // time of publishing
    xml_open_tag(&ctx, "li");
    output_entry_time(&ctx, entry);
    xml_close_tag(&ctx, "li");

    // link back to index
    xml_open_tag(&ctx, "li");
    xml_open_tag_attrs(&ctx, "a", 1, "href", script_name);
    xml_raw(&ctx, "Back");
    xml_close_tag(&ctx, "a");
    xml_close_tag(&ctx, "li");

    xml_close_including(&ctx, "article");
}

void template_index_entry(struct entry entry) {
    // TODO time?
    xml_open_tag_attrs(&ctx, "article", 1, "id", entry.title);
    if(entry.text_size > 0) {
        xml_open_tag_attrs(&ctx, "div", 1, "class", "content index");
        xml_raw(&ctx, entry.text);
        xml_close_tag(&ctx, "div");
    }

    xml_open_tag_attrs(&ctx, "div", 1, "class", "meta index");
    xml_open_tag(&ctx, "p");
    xml_open_tag_attrs(&ctx, "a", 1, "href", entry.link);
    xml_raw(&ctx, "Permalink");

    xml_close_including(&ctx, "article");
}

void template_error(int status) {
    xml_open_tag_attrs(&ctx, "div", 1, "class", "error-page");
    xml_open_tag(&ctx, "h2");
    xml_raw(&ctx, "An error occured while handling your request");
    xml_close_tag(&ctx, "h2");

    xml_open_tag(&ctx, "p");

    if(status == 500) {
        xml_raw(&ctx, "Something is wrong with this application and/or its server (error 500).");
    } else if(status == 404) {
        xml_raw(&ctx, "What you requested doesn't exist (error 404).");
    } else {
       xml_raw(&ctx, "The error encoutered is: ");
       xml_raw(&ctx, http_status_line(status));
    }

    xml_close_tag(&ctx, "p");

    char *script_name = getenv("SCRIPT_NAME");

    xml_open_tag(&ctx, "nav");
    xml_open_tag(&ctx, "p");
    xml_open_tag_attrs(&ctx, "a", 1, "href", script_name);
    xml_raw(&ctx, "Back");

    xml_close_including(&ctx, "div");
}
