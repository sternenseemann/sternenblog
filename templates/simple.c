#include <stdio.h>
#include <stdlib.h>
#include "../template.h"
#include "../config.h"

void template_header(void) {
	printf("<!doctype html>\n"
	       "<html><head>"
	       "<title>simple blog</title><meta charset=\"utf-8\"/>"
	       "<style type=\"text/css\">\n"
	       "a:link, a:visited{\n"
	       "color:blue;\n"
	       "}</style>"
	       "</head>\n"
	       "<body><h1>%s</h1>\n", BLOG_TITLE);
}

void template_footer(void) {
	printf("</body></html>");
}

void template_post_single_entry(struct blogpost post) {
	FILE *fp = fopen(post.path, "r");
	char c;
	
	printf("<article>\n");

	while((c = fgetc(fp)) != EOF) {
		printf("%c", c);
	}
	
	printf("<p><a href=\"%s\">Back</a></p></article>\n", getenv("SCRIPT_NAME"));

	fclose(fp);
}

void template_post_index_entry(struct blogpost post) {
        FILE *fp = fopen(post.path, "r");
        char c;

        printf("<article>\n");

        while((c = fgetc(fp)) != EOF) {
                printf("%c", c);
        }

        printf("<p><a href=\"%s\">Permalink</a></p></article>\n", post.link);

        fclose(fp);
}

void template_error_404(void) {
	printf("<article><h2>Error 404</h2><p>This page does not exist. I am so sorry</p></article>");
}
