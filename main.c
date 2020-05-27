#include <stdio.h>
#include <string.h>
#include "fs_utils.h"
#include "linked_list.h"
#include "errors.h"
#include "log.h"
#include "wc_utils.h"
#include "hash_map.h"


int main() {
    log_set_level(LOG_TRACE);
    log_info("MPi Word Count startup");

    enum wc_error status = NO_ERROR;
    char path[] = "/Users/sergio/ClionProjects/mpi_word_count/test_dir";
    struct LinkedList *files = list_directory(path, &status);
    if (NO_ERROR != status) {
        log_fatal("listing directory failed error_code=%d", status);
        return -1;
    }

    log_debug("found files %d", ll_size(files));

    struct LinkedList **splitted_file_lists = split_files_equally(files, 10, &status);
    if (NO_ERROR != status) {
        log_fatal("filelist splitting failed error_code=%d", status);
        return -2;
    }

    struct WordFreq *result = word_frequencies("/Users/sergio/ClionProjects/mpi_word_count/test_dir/t1.txt", &status);
    print_frequencies(result);

    return 0;
}