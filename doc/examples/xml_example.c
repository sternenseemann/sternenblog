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
    xml_escaped(&ctx, "lol this is my site");
    xml_close_including(&ctx, "head");
    xml_open_tag(&ctx, "body");
    xml_open_tag(&ctx, "header");
    xml_open_tag(&ctx, "h1");
    xml_escaped(&ctx, ">>sophisticated<< technology");
    xml_close_including(&ctx, "header");
    xml_open_tag(&ctx, "main");
    xml_open_tag_attrs(&ctx, "article", 1, "id", "article-1");
    xml_open_tag(&ctx, "h2");
    xml_escaped(&ctx, "i was joking");
    xml_close_tag(&ctx, "h2");
    xml_open_tag(&ctx, "p");
    xml_escaped(&ctx, "it really was only a joke, i was leading you on!");
    xml_close_including(&ctx, "article");
    xml_open_tag_attrs(&ctx, "article", 2, "id", "article-2", "data-meta", "{ \"type\" : \"preformatted\" }");
    xml_open_tag(&ctx, "h2");
    xml_escaped(&ctx, "table test");
    xml_close_tag(&ctx, "h2");
    xml_open_tag(&ctx, "pre");
    xml_escaped(&ctx, "+-----+--------------------+\n");
    xml_escaped(&ctx, "| wow | this aligns right! |\n");
    xml_escaped(&ctx, "+-----+--------------------+\n");
    xml_close_including(&ctx, "article");
    xml_open_tag_attrs(&ctx, "article", 1, "id", "article-3");
    xml_open_tag(&ctx, "h2");
    xml_escaped(&ctx, "escaping \"test\"");
    xml_close_tag(&ctx, "h2");
    xml_escaped(&ctx, "&<>\"\'");
    xml_close_all(&ctx);

    del_xml_context(&ctx);
    return 0;
}
