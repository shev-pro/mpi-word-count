#include <stdio.h>
#include <string.h>
#include <time.h>
#include "fs_utils.h"
#include "linked_list.h"
#include "errors.h"
#include "log.h"
#include "wc_utils.h"
#include "hash_map.h"
#include "wc_constants.h"
#include "wc_mpi_helpers.h"
#include "wc_core.h"
#include <mpi.h>
#include <limits.h>
#include <stdbool.h>

#define IS_MASTER == 0
#define IS_SLAVE > 0


int main(int argc, char *argv[]) {
    log_set_level(LOG_TRACE);
    log_info("MPi Word Count startup");

    MPI_Init(&argc, &argv);

    int numtasks, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    enum wc_error wc_status = NO_ERROR;

    char path[] = "/Users/sergio/ClionProjects/mpi_word_count/test_dir";


    if (rank IS_MASTER) {
        log_debug("Master started");

        /**
         * SPLITTING START
         */
        LinkedList *files = list_directory(path, &wc_status);
        if (NO_ERROR != wc_status) {
            log_fatal("listing directory failed error_code=%d", wc_status);
            return -1;
        }
        log_debug("found files %d", ll_size(files));


        LinkedList **splitted_file_lists = split_files_equally(files, numtasks, &wc_status);
        if (NO_ERROR != wc_status) {
            log_fatal("filelist splitting failed error_code=%d", wc_status);
            return -2;
        }

        /**
         * SHUFFLE
         */
        shuffling_to_slaves(splitted_file_lists, numtasks);

        /**
         * SHUFFLE END
         */
        LinkedList *local_file_list = splitted_file_lists[0];
        WordFreq *local_frequency = worker_process_files(local_file_list, rank, &wc_status);
        if (NO_ERROR != wc_status) {
            log_fatal("local_frequency failed on %d with error %d", rank, wc_status);
        }

        print_frequencies(local_frequency, false);
        printf("MASTER =====\n");

        for (int i = 1; i < numtasks; i++) {
            WordFreqContig words_contig = pull_frequency_results(&wc_status);
            if (NO_ERROR != wc_status) {
                log_fatal("push_frequency_results failed with code %d", wc_status);
            }

            merge_locally(local_frequency, words_contig);
        }

//        print_frequencies(local_frequency, false);

        dump_csv("/tmp/ppp", local_frequency);
    }

    if (rank IS_SLAVE) {
        log_debug("Slave started");

        LinkedList *local_file_list = fetch_work_from_master(&wc_status);
        if (NO_ERROR != wc_status) {
            log_fatal("fetch_work_from_master failed on %d with error %d", rank, wc_status);
        }

        WordFreq *local_frequency = worker_process_files(local_file_list, rank, &wc_status);
        if (NO_ERROR != wc_status) {
            log_fatal("local_frequency failed on %d with error %d", rank, wc_status);
        }

//        print_frequencies(local_frequency, false);
//        printf("SLAVE =====\n");

        push_frequency_results(local_frequency, 0, &wc_status);
        if (NO_ERROR != wc_status) {
            log_fatal("push_frequency_results failed with code %d", wc_status);
        }
    }


    MPI_Finalize();
    return 0;
}