#include <stdio.h>
#include <stdlib.h>
#include "../template.h"
#include "../config.h"

void template_header(void) {
	printf("<!doctype html>\n"
	       "<html>\n"
	       "<head>\n"
	       "\t<title>simple blog</title>\n"
	       "\t<meta charset=\"utf-8\"/>\n"
	       "\t<style type=\"text/css\">\n"
	       "\ta:link, a:visited{\n"
	       "\tcolor:blue;\n"
	       "\t}</style>\n"
	       "</head>\n"
	       "<body><h1>%s</h1>\n", BLOG_TITLE);
}

void template_footer(void) {
	printf("\t<footer><a href=\"%s/rss.xml\">RSS feed</a></footer>\n"
			"</body></html>", getenv("SCRIPT_NAME"));
}

void template_post_single_entry(struct blogpost post) {
	FILE *fp = fopen(post.path, "r");
	char c;

	if(fp == NULL) {
		template_error_404();
		exit(EXIT_SUCCESS);
		/* TODO: does CGI still work correctly if we exit
		 * with EXIT_FAILURE? */
	}
	
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

	if(fp == NULL) {
		fprintf(stderr, "No such file or directory: %s\n", post.path);
		exit(EXIT_FAILURE);
	}

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
