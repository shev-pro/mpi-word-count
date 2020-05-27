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
#include <mpi.h>
#include <limits.h>

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

    char path[] = "/Users/sergio/ClionProjects/mpi_word_count/test_dir";
    struct LinkedList *files = list_directory(path, &wc_status);
    if (NO_ERROR != wc_status) {
        log_fatal("listing directory failed error_code=%d", wc_status);
        return -1;
    }
    char buf_path[PATH_MAX];
    log_debug("found files %d", ll_size(files));
    if (rank IS_MASTER) {
        log_debug("Master started");
        struct LinkedList **splitted_file_lists = split_files_equally(files, numtasks, &wc_status);
        if (NO_ERROR != wc_status) {
            log_fatal("filelist splitting failed error_code=%d", wc_status);
            return -2;
        }
        send_workload_to_slaves(splitted_file_lists, numtasks);
    }
    if (rank IS_SLAVE) {
        log_debug("Slave started");
        struct LinkedList *local_file_list = ll_construct_linked_list();
        int finished = 0;
        while (finished == 0) {
            MPI_Recv(&buf_path, PATH_MAX, MPI_CHAR, MPI_ANY_SOURCE, REGULAR_WORK_TAG, MPI_COMM_WORLD, &status);
            if (strncmp(PATH_RECV_FINISH, buf_path, strlen(PATH_RECV_FINISH)) == 0) {
                finished = 1;
            } else {
                char *rec_path = calloc(strnlen(buf_path, PATH_MAX) + 1, sizeof(char));
                strncpy(rec_path, buf_path, PATH_MAX);
                ll_add_last(local_file_list, rec_path);
            }
        }
        printf("====\n");
        ll_print(local_file_list);
        //        struct WordFreq *result = word_frequencies("/Users/sergio/ClionProjects/mpi_word_count/test_dir/t1.txt",
//                                                   &wc_status);

    }

    MPI_Finalize();
    return 0;
}