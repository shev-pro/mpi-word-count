#include <stdio.h>
#include <string.h>
#include "fs_utils.h"
#include "linked_list.h"
#include "errors.h"
#include "log.h"
#include "wc_utils.h"


int main() {
    log_set_level(LOG_TRACE);
    log_info("MPi Word Count startup");

    enum wc_error status = NO_ERROR;
    char path[] = "/Users/sergio/ClionProjects/mpi_word_count/test_dir";
    struct LinkedList *files = list_directory(path, &status);
    if (status != NO_ERROR) {
        log_fatal("listing directory failed error_code=%d", status);
        return -1;
    }

    log_debug("found files %d", ll_size(files));

    struct LinkedList **splitted_file_lists = split_files_equally(files, 4, &status);
    return 0;
}