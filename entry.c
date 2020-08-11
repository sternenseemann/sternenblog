#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "core.h"
#include "entry.h"

int http_error(int err) {
    switch(err) {
        case EACCES:
            return 403;
        case ENOENT:
            return 404;
        case ENOTDIR:
            return 404;
        default:
            return 500;
    }
}

int make_entry(const char *blog_dir, char *script_name, char *path_info, struct entry *entry) {
    // TODO: allow subdirectories?
    // TODO: no status code return?

    // TODO: url encoding of links

    // intialize pointers
    entry->link = NULL;
    entry->path = NULL;
    entry->title = NULL;

    // won't be handled by make_entry
    entry->text = NULL;
    entry->text_size = 0;

    // validate path_info
    if(path_info == NULL) {
        fprintf(stderr, "Missing PATH_INFO\n");
        return 500;
    }

    size_t path_info_len = strlen(path_info);

    // if path_info is empty make_entry shouldn't be called
    // as per RFC3875 expect it to start with a slash
    if(path_info_len == 0 || path_info[0] != '/') {
        fprintf(stderr, "Malformed PATH_INFO: \"%s\"\n", path_info);
        return 400;
    }

    // check if the path_info segments are alright
    // should be sane coming from a webserver
    int last_was_slash = 0;
    for(size_t i = 0; i < path_info_len; i++) {
        if(last_was_slash) {
            switch(path_info[i]) {
                case '/':
                    // TODO: necessary?
                    fprintf(stderr, "Double slash in PATH_INFO: \"%s\"\n", path_info);
                    return 400;
                    break;
                case '.':
                    fprintf(stderr, "Dot file or dir in PATH_INFO: \"%s\"\n", path_info);
                    return 403;
                    break;
                default:
                    last_was_slash = 0;
            }
        } else if(path_info[i] == '/') {
            last_was_slash = 1;
        }
    }

    // set title (PATH_INFO without the slash)
    if(path_info_len < 2) {
        // shouldn't be called with just "/"
        return 500;
    }

    // title length is exactly path_info_len (-1 for slash, +1 for null byte)
    entry->title = malloc(sizeof(char) * path_info_len);
    memcpy(entry->title, path_info + 1, sizeof(char) * path_info_len);

    // build path to entry's file
    size_t blog_dir_len = strlen(blog_dir);

    entry->path = malloc(sizeof(char) * (path_info_len + blog_dir_len + 1));

    memcpy(entry->path, blog_dir, blog_dir_len * sizeof(char));

    // prevent double slash
    if(entry->path[blog_dir_len - 1] == '/') {
        blog_dir_len--;
    }

    memcpy(entry->path + blog_dir_len, path_info, path_info_len);
    entry->path[path_info_len + blog_dir_len] = '\0';

    struct stat file_info;
    memset(&file_info, 0, sizeof(struct stat));

    if(stat(entry->path, &file_info) == -1) {
        return http_error(errno);
    }

    int regular_file = (file_info.st_mode & S_IFMT) == S_IFREG;

    // refuse to process files that are not
    // owned by the webserver's group or user
    gid_t gid = getegid();
    uid_t uid = geteuid();
    int access = file_info.st_gid == gid || file_info.st_uid == uid;

    if(!access) {
        return http_error(EACCES);
    } else if(!regular_file) {
        return http_error(ENOENT);
    }

    // use POSIX compatible version, since we don't need nanoseconds
    entry->time = file_info.st_mtime;

    // build the link using SCRIPT_NAME
    if(script_name == NULL) {
        fprintf(stderr, "Missing SCRIPT_NAME\n");
        return 500;
    }

    // don't check SCRIPT_NAME validity, since we
    // don't depend on it starting with a slash

    size_t script_name_len = strlen(script_name);

    entry->link = malloc(sizeof(char) * (script_name_len + path_info_len + 1));

    if(script_name_len != 0) {
        memcpy(entry->link, script_name, script_name_len);
    }

    memcpy(entry->link + script_name_len, path_info, path_info_len);

    entry->link[path_info_len + script_name_len] = '\0';

    return 200;
}

int entry_get_text(struct entry *entry) {
    // TODO set errno correctly in all cases
    if(entry->text != NULL) {
        // nothing to do
        return 0;
    }

    int fd = open(entry->path, O_RDONLY);

    if(fd == -1) {
        return -1;
    }

    struct stat file_info;

    if(fstat(fd, &file_info) == -1) {
        return -1;
    }

    if(file_info.st_size == 0) {
        close(fd);
        return 0;
    }

    entry->text = mmap(NULL, file_info.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    if(entry->text == MAP_FAILED) {
        entry->text = NULL;
        close(fd);
        return -1;
    }

    entry->text_size = file_info.st_size;

    if(close(fd) == -1) {
        return -1;
    }

    return 0;
}

void free_entry(struct entry entry) {
    if(entry.path != NULL) {
        free(entry.path);
    }

    if(entry.link != NULL) {
        free(entry.link);
    }

    if(entry.title != NULL) {
        free(entry.title);
    }

    if(entry.text != NULL) {
        // TODO return value
        munmap(entry.text, entry.text_size);
    }
}
