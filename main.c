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

    MPI_Status status;
    MPI_Init(&argc, &argv);

    int numtasks, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    enum wc_error wc_status = NO_ERROR;

    struct HashTable *t1 = ht_create_table(500);
    ht_insert_str(t1, "pippo", "123");


    if (rank IS_MASTER) {
        log_debug("Master started");
        char path[] = "/Users/sergio/ClionProjects/mpi_word_count/test_dir";
        struct LinkedList *files = list_directory(path, &wc_status);
        if (NO_ERROR != wc_status) {
            log_fatal("listing directory failed error_code=%d", wc_status);
            return -1;
        }

        log_debug("found files %d", ll_size(files));

        struct LinkedList **splitted_file_lists = split_files_equally(files, numtasks, &wc_status);
        if (NO_ERROR != wc_status) {
            log_fatal("filelist splitting failed error_code=%d", wc_status);
            return -2;
        }
        send_workload_to_slaves(splitted_file_lists, numtasks);
        struct LinkedList *local_file_list = splitted_file_lists[0];
        struct WordFreq *local_frequency = worker_process_files(local_file_list, rank, &wc_status);

        char *words_joined = 0;
        size_t words_joined_len = 0;
        int *frequency_arr = 0;
        size_t frequency_arr_len = 0;

        wc_status = wc_dump(local_frequency, &words_joined, &words_joined_len, &frequency_arr, &frequency_arr_len);

        printf("%s\n", words_joined);

        for (int i = 0; i < frequency_arr_len; i++) {
            printf("%d \n", frequency_arr[i]);
        }
        printf("\n");
        if (NO_ERROR != wc_status) {
            log_fatal("local_frequency failed on %d with error %d", rank, wc_status);
        }

//        print_frequencies(local_frequency, true);
    }

    if (rank IS_SLAVE) {
        log_debug("Slave started");
        char buf_path[PATH_MAX];
        struct LinkedList *local_file_list = ll_construct_linked_list();
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

        struct WordFreq *local_frequency = worker_process_files(local_file_list, rank, &wc_status);
        if (NO_ERROR != wc_status) {
            log_fatal("local_frequency failed on %d with error %d", rank, wc_status);
        }

//        print_frequencies(local_frequency, true);
    }


    MPI_Finalize();
    return 0;
}