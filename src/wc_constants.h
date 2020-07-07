//
// Created by Sergio Shevchenko on 26/05/2020.
//

#ifndef MPI_WORD_COUNT_WC_CONSTANTS_H
#define MPI_WORD_COUNT_WC_CONSTANTS_H

#define POSIX_MAX_FILE_NAME 256 // Actually 255 but lets consider final \0
#define POSIX_MAX_PATH_LEN 4096
#define WF_HT_SIZE 25000
#define FILE_DISTR_TAG 1
#define FILE_DISTR_TAG_FINAL 2
#define RESULT_REDUCE_TAG 3
#define NO_WORK_SORRY 4
#define MAX_WORD_SIZE 150
#define PATH_RECV_FINISH "{{FINISHED}}"
#endif //MPI_WORD_COUNT_WC_CONSTANTS_H
