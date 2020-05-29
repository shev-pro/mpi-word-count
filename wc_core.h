//
// Created by Sergio Shevchenko on 28/05/2020.
//

#ifndef MPI_WORD_COUNT_WC_CORE_H
#define MPI_WORD_COUNT_WC_CORE_H

#include "linked_list.h"
#include "wc_utils.h"
#include "log.h"

static struct WordFreq *worker_process_files(LinkedList *local_file_list, int rank, enum wc_error *wc_status) {
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

static LinkedList *fetch_work_from_master(enum wc_error *wc_status) {
    *wc_status = NO_ERROR;

    MPI_Status status;
    char buf_path[PATH_MAX];
    LinkedList *local_file_list = ll_construct_linked_list();
    bool finished = false;
    while (finished == false) {
        MPI_Recv(&buf_path, PATH_MAX, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        log_debug("Received %s tag %d", buf_path, status.MPI_TAG);

        if (status.MPI_TAG == FILE_DISTR_TAG_FINAL) {
            finished = true;
        }
        int received_path_len = (int) (strnlen(buf_path, PATH_MAX) + 1);
        char *rec_path = calloc((size_t) received_path_len, sizeof(char));
        strcpy(rec_path, buf_path);
        memset(buf_path, 0x00, PATH_MAX);
        ll_add_last(local_file_list, rec_path);
    }

    return local_file_list;
}

#endif //MPI_WORD_COUNT_WC_CORE_H
