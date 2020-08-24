#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "core.h"
#include "entry.h"
#include "index.h"

/*!
 * @brief Base size of the allocated index array
 *
 * Base count of `struct index` to use for allocation
 * of the `*entries` array in `make_index()`.
 *
 * @see make_index
 */
#define BASE_INDEX_SIZE 64

int entries_timesort_r(struct entry *a, struct entry *b) {
    if(a == NULL && b == NULL) {
        return 0;
    } else if(a == NULL) {
        return 1;
    } else if(b == NULL) {
        return -1;
    } else if(a->time > b->time) {
        return -1;
    } else if(a->time < b->time) {
        return 1;
    } else {
        return 0;
    }
}

int make_index(const char *blog_dir, char *script_name, bool get_text, struct entry *entries[]) {
    if(*entries != NULL) {
        return -1;
    }

    if(script_name == NULL) {
        return -1;
    }

    size_t index_count = 0;
    DIR *dir;

    dir = opendir(blog_dir);

    if(dir == NULL) {
        return -1;
    }

    struct dirent *ent;

    size_t size = BASE_INDEX_SIZE;
    *entries = malloc(sizeof(struct entry) * size);

    if(*entries == NULL) {
        return -1;
    }

    // losely based on musl's scandir(3)
    // directly use struct entry instead of dirents,
    // so we can directly use entries for filtering
    // and later sorting
    //
    // TODO errno handling
    while((ent = readdir(dir)) != NULL) {
        if(ent->d_name[0] != '.') {
            // build PATH_INFO for given entry
            size_t d_name_len = strlen(ent->d_name);
            char path_info[d_name_len + 2];
            path_info[0] = '/';
            memcpy(path_info + 1, ent->d_name, d_name_len + 1);

            struct entry tmp_entry;

            int result = make_entry(blog_dir, script_name, path_info, &tmp_entry);

            if(result == 200) {
                // increase array size if necessary
                if(index_count >= size) {
                    size += BASE_INDEX_SIZE;

                    if(size > SIZE_MAX/sizeof(struct entry)) {
                        break;
                    }

                    struct entry *tmp = realloc(*entries, size * sizeof(struct entry));

                    if(tmp == NULL) {
                        break;
                    }

                    *entries = tmp;
                }

                memcpy(*entries + index_count, &tmp_entry, sizeof(struct entry));

                if(get_text) {
                    entry_get_text(*entries + index_count);
                }

                index_count++;
            } else {
                free_entry(tmp_entry);
            }
        }
    }

    closedir(dir);

    // reverse sort by time (use "incorrect" compar function
    // to avoid using glibc specific qsort_r)
    qsort(*entries, index_count, sizeof(struct entry),
          (int (*)(const void *, const void *)) entries_timesort_r);

    return index_count;
}

void free_index(struct entry *entries[], int count) {
    if(count > 0) {
        for(int i = 0; i < count; i++) {
            free_entry(*(*entries + i));
        }
    }

    free(*entries);
}
