#include <stdio.h>
#include <stdlib.h>
#include "../template.h"

#define TITLE "blog in C"

void template_header(void) {
	printf("<!doctype html>\n"
	       "<html><head>"
	       "<title>simple blog</title><meta charset=\"utf-8\"/>"
	       "<style type=\"text/css\">\n"
	       "a:link, a:visited{\n"
	       "color:blue;\n"
	       "}</style>"
	       "</head>\n"
	       "<body><h1>%s</h1>\n", TITLE);
}

void template_footer(void) {
	printf("</body></html>");
}

void template_post_single_entry(char post_path[]) {
	FILE *fp = fopen(post_path, "r");
	char c;
	
	printf("<article>\n");

	while((c = fgetc(fp)) != EOF) {
		printf("%c", c);
	}
	
	printf("<p><a href=\"%s\">Back</a></p></article>\n", getenv("SCRIPT_NAME"));

	fclose(fp);
}

void template_post_index_entry(char post_path[], char link_path[]) {
        FILE *fp = fopen(post_path, "r");
        char c;

        printf("<article>\n");

        while((c = fgetc(fp)) != EOF) {
                printf("%c", c);
        }

        printf("<p><a href=\"%s\">Permalink</a></p></article>\n", link_path);

        fclose(fp);
}

void template_error_404(void) {
	printf("<article><h2>Error 404</h2><p>This page does not exist. I am so sorry</p></article>");
}
