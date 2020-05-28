//
// Created by Sergio Shevchenko on 27/05/2020.
//

#ifndef MPI_WORD_COUNT_WC_MPI_HELPERS_H
#define MPI_WORD_COUNT_WC_MPI_HELPERS_H

#include <mpi.h>
#include <limits.h>
#include "linked_list.h"
#include "wc_constants.h"
#include "log.h"

static void send_workload_to_slaves(struct LinkedList **splitted_file_lists, int numtasks) {
    for (int slave_rank = 1; slave_rank < numtasks; slave_rank++) {
        struct LinkedList *slave_files_list = splitted_file_lists[slave_rank];
        int tag = FILE_DISTR_TAG;

        for (int j = 0; j < ll_size(slave_files_list); j++) {
            char *path = ll_find(slave_files_list, j)->data;
            log_debug("Master -> Slave %d Sending path %s", slave_rank, path);
            if (j == ll_size(slave_files_list) - 1) {
                tag = FILE_DISTR_TAG_FINAL;
            }
            MPI_Send(path, (int) strnlen(path, PATH_MAX), MPI_CHAR, slave_rank, tag, MPI_COMM_WORLD);
        }
        log_debug("Master - Slave %d Sending finish message", slave_rank);
    }
}

#endif //MPI_WORD_COUNT_WC_MPI_HELPERS_H
