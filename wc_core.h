//
// Created by Sergio Shevchenko on 28/05/2020.
//

#ifndef MPI_WORD_COUNT_WC_CORE_H
#define MPI_WORD_COUNT_WC_CORE_H

#include "linked_list.h"
#include "wc_utils.h"
#include "log.h"

static void worker_process_files(struct LinkedList *local_file_list, int rank, enum wc_error *wc_status) {
    ll_print(local_file_list);
    struct WordFreq *result = NULL;

    for (int i = 0; i < ll_size(local_file_list); i++) {
        char *path_to_count = ll_find(local_file_list, i)->data;
        result = word_frequencies(result, path_to_count, wc_status);
        if (NO_ERROR != *wc_status) {
            log_error("Huston, we have an error with code %d", wc_status);
            return;
        }
    }
//    print_frequencies(result, true);

    log_debug("Frequencies %d", rank);
}

#endif //MPI_WORD_COUNT_WC_CORE_H
