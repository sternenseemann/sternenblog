/* blog
 * A small file based blog software
 * intended to run as a CGI script
 * written in C by Lukas Epple aka
 * sternenseemann */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include "template.h"

#define BLOGDIR "./posts"

/* generates the template
 * for the index page */
void blog_index(void);

/* generates the template for 
 * a single view of a blog post*/
void blog_post(char post_path[]);

/* checks if a file exists
 * returns 0 if not
 * returns 1 if
 * returns -1 if the file isn't accesible
 * for some other reason */
int file_exists(char path[]);

/* sends a CGI/HTTP header */
void send_header(char key[], char val[]);

/* terminates the header section of a
 * CGI/HTTP Response */
void terminate_headers(void);

/* function that filters out the .files
 * for scandir */
int no_dotfiles(const struct dirent *file);

int main(void) {
	char *path_info = getenv("PATH_INFO");
	
	if(path_info == NULL || path_info[0] == '\0' || strcmp(path_info, "/") == 0) {
		blog_index();
	} else {
		unsigned long bufsize = strlen(BLOGDIR) + strlen(path_info);
		char post_path[bufsize];
		strcpy(post_path, BLOGDIR);
		strcat(post_path, path_info);

		blog_post(post_path);
	}

	return EXIT_SUCCESS;
}

/************************
 * wrapper functions for
 * template generation
 ***********************/

void blog_index(void) {
	char *script_name = getenv("SCRIPT_NAME");
	struct dirent **dirlist;
	int dircount;

	if(script_name == NULL) {
		fprintf(stderr, "Died because of missing self-awareness\n");
		exit(EXIT_FAILURE);
	}

	send_header("Content-type", "text/html");
	terminate_headers();

	template_header();

	dircount = scandir(BLOGDIR, &dirlist, no_dotfiles, alphasort);	

	if(dircount < 0) {
		fprintf(stderr, "An error occurred while scanning %s: %s\n", 
				BLOGDIR, strerror(errno));
		exit(EXIT_FAILURE);
	} else {
		while(dircount--) {
			/* first create the path to the blogpost
			 * which is passed to the template function */
			unsigned long bufsize = strlen(BLOGDIR)
				+ 1 + strlen(dirlist[dircount]->d_name);
			char post_path[bufsize];
			strcpy(post_path, BLOGDIR);
			strcat(post_path, "/");
			strcat(post_path, dirlist[dircount]->d_name);
		
			/* then create the link URL which is used
			 * for the permalink */
			bufsize = strlen(script_name) + 1 +
			strlen(dirlist[dircount]->d_name);
			char link_path[bufsize];
			strcpy(link_path, script_name);
			strcat(link_path, "/");
			strcat(link_path, dirlist[dircount]->d_name);
			
			/* finally if the file exists call the
			 * template function. Otherwise
			 * we do nothing. (this case is also
			 * FUCKING unlikely */
			if(file_exists(post_path) > 0) {
				template_post_index_entry(post_path, link_path);
			}

			free(dirlist[dircount]);
		}
		free(dirlist);
	}

	template_footer();
}

void blog_post(char post_path[]) {
	if(file_exists(post_path) > 0) {
		send_header("Content-type", "text/html");
		terminate_headers();

		template_header();
		template_post_single_entry(post_path);
	} else {
		send_header("Content-type", "text/html");
		send_header("Status", "404 Not Found");
		terminate_headers();

		template_header();
		template_error_404();
	}

	template_footer();
}

int file_exists(char path[]) {
	FILE *fp = fopen(path, "r");

	if(fp == NULL && errno == ENOENT) {
		return 0;
	} else if(fp == NULL) {
		// some other error occured
		return -1;
	} else {
		fclose(fp);
		return 1;
	}
}

/*********************
 * header utilities 
 ********************/
void send_header(char key[], char val[]) {
	printf("%s: %s\n", key, val);
}

void terminate_headers(void) {
	printf("\n");
}

/******************************
 * filter function
 * for scandir(3)
 *****************************/

int no_dotfiles(const struct dirent *file) {
	if(file->d_name[0] == '.') {
		return 0;
	} else {
		return 1;
	}
}
