#include <stdio.h>
#include <xml.h>

int main() {
    struct xml_context ctx;
    new_xml_context(&ctx);

    ctx.closing_slash = 0;

    // Uncomment for debugging warnings:
    // ctx.warn = stderr;

    xml_raw(&ctx, "<!doctype html>");
    xml_open_tag_attrs(&ctx, "html", 1, "lang", "en");
    xml_open_tag(&ctx, "head");
    xml_empty_tag(&ctx, "meta", 1, "charset", "utf-8");
    xml_open_tag(&ctx, "title");
    xml_raw(&ctx, "lol this is my site");
    xml_close_including(&ctx, "head");
    xml_open_tag(&ctx, "body");
    xml_open_tag(&ctx, "header");
    xml_open_tag(&ctx, "h1");
    xml_raw(&ctx, "sophisticated technology");
    xml_close_including(&ctx, "header");
    xml_open_tag(&ctx, "main");
    xml_open_tag(&ctx, "article");
    xml_open_tag(&ctx, "h2");
    xml_raw(&ctx, "i was joking");
    xml_close_tag(&ctx, "h2");
    xml_open_tag(&ctx, "p");
    xml_raw(&ctx, "it really was only a joke, i was leading you on!");
    xml_close_including(&ctx, "article");
    xml_open_tag(&ctx, "article");
    xml_open_tag(&ctx, "pre");
    xml_raw(&ctx, "+-----+--------------------+\n");
    xml_raw(&ctx, "| wow | this aligns right! |\n");
    xml_raw(&ctx, "+-----+--------------------+\n");
    xml_close_all(&ctx);

    del_xml_context(&ctx);
    return 0;
}
