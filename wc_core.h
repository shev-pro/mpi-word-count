//
// Created by Sergio Shevchenko on 28/05/2020.
//

#ifndef MPI_WORD_COUNT_WC_CORE_H
#define MPI_WORD_COUNT_WC_CORE_H

#include "linked_list.h"
#include "wc_utils.h"
#include "log.h"

static struct WordFreq *worker_process_files(struct LinkedList *local_file_list, int rank, enum wc_error *wc_status) {
    log_info("worker_process_files [local_file_list=%d, rank=%d] started", ll_size(local_file_list), rank);

    struct WordFreq *result = NULL;

    for (int i = 0; i < ll_size(local_file_list); i++) {
        char *path_to_count = ll_find(local_file_list, i)->data;
        result = word_frequencies(result, path_to_count, wc_status);
        if (NO_ERROR != *wc_status) {
            log_error("Huston, we have an error with code %d", *wc_status);
            return NULL;
        }
    }

    log_info("worker_process_files [local_file_list=%d, rank=%d] finished", ll_size(local_file_list), rank);
    return result;
}

#endif //MPI_WORD_COUNT_WC_CORE_H
