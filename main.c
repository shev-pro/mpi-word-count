#include <stdio.h>
#include <string.h>
#include <time.h>
#include "fs_utils.h"
#include "linked_list.h"
#include "errors.h"
#include "log.h"
#include "wc_utils.h"
#include "hash_map.h"
#include <mpi.h>

#define IS_MASTER == 0
#define IS_SLAVE > 0
#define REGULAR_WORK_TAG 1

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
    char buffer[4096];
    log_debug("found files %d", ll_size(files));
    if (rank IS_MASTER) {
        log_debug("Master started");
        struct LinkedList **splitted_file_lists = split_files_equally(files, numtasks, &wc_status);
        if (NO_ERROR != wc_status) {
            log_fatal("filelist splitting failed error_code=%d", wc_status);
            return -2;
        }
        for (int i = 1; i < numtasks; i++) {
            size_t buff_size;
            char *joined_str = ll_join(splitted_file_lists[i], '|', &buff_size);
            MPI_Send(joined_str, (int) buff_size, MPI_CHAR, i, REGULAR_WORK_TAG, MPI_COMM_WORLD);
        }
    }
    if (rank IS_SLAVE) {
        log_debug("Slave started");

        MPI_Recv(&buffer, 4096, MPI_CHAR, MPI_ANY_SOURCE, REGULAR_WORK_TAG, MPI_COMM_WORLD, &status);
        printf("Buf %s", buffer);
//        struct WordFreq *result = word_frequencies("/Users/sergio/ClionProjects/mpi_word_count/test_dir/t1.txt",
//                                                   &wc_status);

    }

    MPI_Finalize();
    return 0;
}