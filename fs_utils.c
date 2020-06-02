//
// Created by Sergio Shevchenko on 26/05/2020.
//

#include "fs_utils.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"
#include "wc_constants.h"
#include "log.h"

#include <string.h>
#include <sys/stat.h>

LinkedList *list_directory(const char *dir_path, enum wc_error *error) {
    log_debug("list_directory %s", dir_path);

    DIR *d;
    struct dirent *dir;
    LinkedList *file_list = NULL;
    d = opendir(dir_path);
    if (d) {
        file_list = ll_construct_linked_list();
        while ((dir = readdir(d)) != NULL) {
            log_trace("list_directory [dir_path=%s] found %s", "ss", dir->d_name);
            if (strncmp(dir->d_name, "..", 2) != 0 && strncmp(dir->d_name, ".", 1) != 0) {
                char *file_name = malloc(sizeof(char) * POSIX_MAX_FILE_NAME + strnlen(dir_path, POSIX_MAX_PATH_LEN));
                sprintf(file_name, "%s/%s", dir_path, dir->d_name);
                ll_add_last(file_list, file_name);
            }
        }
        closedir(d);
    } else {
        log_error("list_directory [dir_path=%s] failed due IO", dir_path);
        *error = IO_ERROR;
    }
    return file_list;
}

size_t file_size(const char *file_path, enum wc_error *error) {
    struct stat st;
    int res = stat(file_path, &st);
    if (res != 0) {
        log_error("file_size [file_path=%s] failed", file_path);
        *error = IO_ERROR;
    }
    return (size_t) st.st_size;
}

enum wc_error dump_csv(char *file_path, const WordFreq *frequncies) {
    log_debug("dump_csv [filepath=%s]", file_path);

    FILE *fp = fopen(file_path, "w");
    if (fp == NULL) {
        return IO_ERROR;
    }
    fprintf(fp, "%s,%s\n", "word", "frequency");
//    LinkedList *words = frequncies->word_list;
//    HashTable *freq = frequncies->word_frequencies;
//    struct Node *current = ll_next(words, NULL);
//    while (current != NULL) {
//        fprintf(fp, "%s,%d\n", (char *) current->data, *(int *) ht_lookup_str(freq, (char *) current->data));
//        current = ll_next(words, current);
//    }
    fflush(fp);
    fclose(fp);
    return NO_ERROR;
}
