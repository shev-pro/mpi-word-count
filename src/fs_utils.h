//
// Created by Sergio Shevchenko on 26/05/2020.
//

#ifndef MPI_WORD_COUNT_FS_UTILS_H
#define MPI_WORD_COUNT_FS_UTILS_H

#include <stdio.h>
#include "errors.h"
#include "linked_list.h"
#include "wc_utils.h"


LinkedList *list_directory(const char *dir_path, enum wc_error *error);
size_t file_size(const char *file_path, enum wc_error *error);
enum wc_error dump_csv(char *file_path, const WordFreq *frequncies);

#endif //MPI_WORD_COUNT_FS_UTILS_H
