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

#define IS_MASTER == 0
#define IS_SLAVE > 0

int rank; // Global variable needed by logger to print rank

int main(int argc, char *argv[]) {
    log_set_level(LOG_INFO);
    if (argc < 3) {
        log_error("Invocation should be ./mpi_word_count <src_directory> <result_file>");
        return -1;
    }
    char *from_directory = argv[1];
    char *to_file = argv[2];
    log_info("Fetching data from %s, writing results to %s", from_directory, to_file);
    log_info("MPi Word Count startup");

    MPI_Init(&argc, &argv);

    int workers;

    MPI_Comm_size(MPI_COMM_WORLD, &workers);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    enum wc_error wc_status = NO_ERROR;

    if (rank IS_MASTER) {
        log_debug("Master started");

        /**
         * SPLITTING START
         */
        LinkedList *files = list_directory(from_directory, &wc_status);
        if (NO_ERROR != wc_status) {
            log_fatal("listing directory failed error_code=%d", wc_status);
            goto finalize;
        }
        log_debug("found files %d", ll_size(files));

        unsigned int actual_workers = 0;
        LinkedList **splitted_file_lists = split_files_equally(files, workers, &actual_workers, &wc_status);
        if (NO_ERROR != wc_status) {
            log_fatal("filelist splitting failed error_code=%d", wc_status);
            goto finalize;
        }

        /**
         * SHUFFLE
         */
        push_work_to_workers(splitted_file_lists, workers);

        /** PROCESSING ON MASTER
         */
        LinkedList *local_file_list = splitted_file_lists[0];
        WordFreq *local_frequency = worker_process_files(local_file_list, rank, &wc_status);
        if (NO_ERROR != wc_status) {
            log_fatal("local_frequency failed on %d with error %d", rank, wc_status);
            goto finalize;
        }
//        print_frequencies(local_frequency, false);
        for (int i = 1; i < actual_workers; i++) {
            WordFreqContig words_contig = pull_frequency_results(&wc_status);
            if (NO_ERROR != wc_status) {
                log_fatal("push_frequency_results failed with code %d", wc_status);
                goto finalize;
            }

            merge_locally(local_frequency, words_contig);
        }

        dump_csv(to_file, local_frequency);
    }

    if (rank IS_SLAVE) {
        log_debug("Slave started");

        LinkedList *local_file_list = fetch_work_from_master(&wc_status);
        if (NO_ERROR != wc_status) {
            log_fatal("fetch_work_from_master failed on %d with error %d", rank, wc_status);
            goto finalize;
        }

        WordFreq *local_frequency = worker_process_files(local_file_list, rank, &wc_status);
        if (NO_ERROR != wc_status) {
            log_fatal("local_frequency failed on %d with error %d", rank, wc_status);
            goto finalize;
        }

        push_frequency_results(local_frequency, 0, &wc_status);
        if (NO_ERROR != wc_status) {
            log_fatal("push_frequency_results failed with code %d", wc_status);
            goto finalize;
        }
    }

    finalize:
    MPI_Finalize();

    return 0;
}