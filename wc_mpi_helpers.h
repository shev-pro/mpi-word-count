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

static void shuffling_to_slaves(LinkedList **splitted_file_lists, int numtasks) {
    for (int slave_rank = 1; slave_rank < numtasks; slave_rank++) {
        LinkedList *slave_files_list = splitted_file_lists[slave_rank];
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

static void push_frequency_results(WordFreq *freq, int to_rank, enum wc_error *wc_status) {
    log_info("push_frequency_results [freq.words=%d, to_rank=%d]", ll_size(freq->word_list));

    WordFreqContig *dumped = wc_dump(freq, wc_status);
    if (NO_ERROR != *wc_status) {
        log_error("wc dump failed with code %d", *wc_status);
    }
    int position = 0;
    int buffer_max_size = (int) ((dumped->frequencies_len * sizeof(int)) +    // frequency_buffer
                                 (dumped->word_len * sizeof(char)) +          // words_buffer
                                 sizeof(int) +                              // frequency_counter
                                 sizeof(int) +                              // words_counter
                                 1                                          // happy byte
    );

    char *packed_buffer = malloc((size_t) buffer_max_size); // TODO free this

    MPI_Pack(&dumped->frequencies_len, 1, MPI_INT, packed_buffer, buffer_max_size, &position,
             MPI_COMM_WORLD);
    MPI_Pack(&dumped->word_len, 1, MPI_INT, packed_buffer, buffer_max_size, &position,
             MPI_COMM_WORLD);
    MPI_Pack(dumped->words, (int) dumped->word_len, MPI_CHAR, packed_buffer, buffer_max_size, &position,
             MPI_COMM_WORLD);
    MPI_Pack(dumped->frequencies, (int) dumped->frequencies_len, MPI_INT, packed_buffer, buffer_max_size, &position,
             MPI_COMM_WORLD);
    MPI_Send(packed_buffer, position, MPI_PACKED, to_rank, RESULT_REDUCE_TAG, MPI_COMM_WORLD);

    free(packed_buffer);
}

static WordFreqContig pull_frequency_results(enum wc_error *wc_status) {
    MPI_Status status;
    int message_size;
    MPI_Probe(MPI_ANY_SOURCE, RESULT_REDUCE_TAG, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_PACKED, &message_size);

    WordFreqContig dumped;
    char *packed_buffer = malloc((size_t) message_size);

    if (NULL == packed_buffer) {
        *wc_status = OOM_ERROR;
        log_error("pull_frequency_results - OOM error on packed_buffer allocation");
        return dumped;
    }

    MPI_Recv(packed_buffer, message_size, MPI_PACKED, MPI_ANY_SOURCE, RESULT_REDUCE_TAG, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    int position = 0;


    MPI_Unpack(packed_buffer, message_size, &position, &dumped.frequencies_len, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(packed_buffer, message_size, &position, &dumped.word_len, 1, MPI_INT, MPI_COMM_WORLD);

    log_debug("pull_frequency_results [frequencies_len=%d, words_len=%d]", (int) dumped.frequencies_len,
              (int) dumped.word_len);

    void *dyn_buffer_frequencies = malloc((int)dumped.frequencies_len * sizeof(int));
    if (NULL == dyn_buffer_frequencies) {
        *wc_status = OOM_ERROR;
        log_error("pull_frequency_results - OOM error on frequencies allocation");
        free(packed_buffer);
        return dumped;
    }

    char *dyn_buffer_words = malloc((int) (dumped.word_len) * sizeof(char));
    if (NULL == dyn_buffer_words) {
        *wc_status = OOM_ERROR;
        log_error("pull_frequency_results - OOM error on words allocation");
        free(dyn_buffer_frequencies);
        free(packed_buffer);
        return dumped;
    }

    MPI_Unpack(packed_buffer, message_size, &position, dyn_buffer_words, (int) dumped.word_len, MPI_CHAR,
               MPI_COMM_WORLD);
    MPI_Unpack(packed_buffer, message_size, &position, dyn_buffer_frequencies, (int) dumped.frequencies_len, MPI_INT,
               MPI_COMM_WORLD);

    dumped.words = dyn_buffer_words;
    dumped.frequencies = dyn_buffer_frequencies;

    free(packed_buffer);
    return dumped;
}

#endif //MPI_WORD_COUNT_WC_MPI_HELPERS_H
