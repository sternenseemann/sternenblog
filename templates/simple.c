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
        xml_escaped(ctx, strtime);
        xml_close_tag(ctx, "time");
    }
}

void template_header(struct template_data data) {
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
    xml_escaped(&ctx, BLOG_TITLE);
    if(data.page_type == PAGE_TYPE_ENTRY) {
       xml_escaped(&ctx, ": ");
       xml_escaped(&ctx, data.entry->title);
    } else if(data.page_type == PAGE_TYPE_ERROR) {
       xml_escaped(&ctx, ": error");
    }
    xml_close_tag(&ctx, "title");

    xml_close_tag(&ctx, "head");

    xml_open_tag(&ctx, "body");
    xml_open_tag(&ctx, "header");
    xml_open_tag(&ctx, "h1");
    if(data.page_type != PAGE_TYPE_INDEX) {
      char *index;
      if(data.script_name == NULL || data.script_name[0] == '\0') {
        index = "/";
      } else {
        index = data.script_name;
      }

      xml_open_tag_attrs(&ctx, "a", 1, "href", index);
    }
    xml_escaped(&ctx, BLOG_TITLE);
    xml_close_including(&ctx, "header");

    xml_open_tag(&ctx, "main");
}

void template_footer(struct template_data data) {
    xml_close_tag(&ctx, "main");

    xml_open_tag(&ctx, "footer");

    char *atom_link = catn_alloc(2, data.script_name, "/atom.xml");

    if(atom_link != NULL) {
        xml_open_tag_attrs(&ctx, "a", 1, "href", atom_link);
        xml_escaped(&ctx, "Atom Feed");
        xml_close_tag(&ctx, "a");

        free(atom_link);
    }

    xml_close_all(&ctx);

    del_xml_context(&ctx);
}

void template_main(struct template_data data) {
    if(data.page_type == PAGE_TYPE_ERROR) {
       xml_open_tag_attrs(&ctx, "div", 1, "class", "error-page");
       xml_open_tag(&ctx, "h2");
       xml_escaped(&ctx, "An error occured while handling your request");
       xml_close_tag(&ctx, "h2");

       xml_open_tag_attrs(&ctx, "div", 1, "class", "content");
       xml_open_tag(&ctx, "p");

       if(data.status == 500) {
          xml_escaped(&ctx, "Something is wrong with this application and/or its server (error 500).");
       } else if(data.status == 404) {
          xml_escaped(&ctx, "What you requested doesn't exist (error 404).");
       } else {
          xml_escaped(&ctx, "The error encoutered is: ");
          xml_escaped(&ctx, http_status_line(data.status));
       }

       xml_close_tag(&ctx, "p");
       xml_close_tag(&ctx, "div");
       xml_close_tag(&ctx, "div");
    } else {

       xml_open_tag(&ctx, "article");

       xml_open_tag(&ctx, "h2");
       if(data.page_type == PAGE_TYPE_INDEX) {
          xml_open_tag_attrs(&ctx, "a", 1, "href", data.entry->link);
       }
       xml_escaped(&ctx, data.entry->title);
       xml_close_including(&ctx, "h2");

       if(data.entry->text_size > 0) {
          xml_open_tag_attrs(&ctx, "div", 1, "class", "content");
          xml_raw(&ctx, data.entry->text);
          xml_close_tag(&ctx, "div");
       }

       xml_open_tag_attrs(&ctx, "div", 1, "class", "meta");

       // modification time
       xml_open_tag_attrs(&ctx, "p", 1, "class", "mtime");
       output_entry_time(&ctx, *data.entry);
       xml_close_tag(&ctx, "p");

       xml_close_including(&ctx, "article");
    }
}
