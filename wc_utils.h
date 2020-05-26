//
// Created by Sergio Shevchenko on 27/05/2020.
//

#ifndef MPI_WORD_COUNT_WC_UTILS_H
#define MPI_WORD_COUNT_WC_UTILS_H

#include "errors.h"

/**
 * Will group file_list into number of groups using ApproxLoadDec strategy with T < 3/4(T*) guarantee
 * @param file_list LinkedList of files (contains char*)
 * @param groups    Groups file_list will be devided to
 * @param status    Overall status
 * @return          Array of LinkedLists long groups
 */
struct LinkedList **split_files_equally(struct LinkedList *file_list, unsigned int groups, enum wc_error *status);

#endif //MPI_WORD_COUNT_WC_UTILS_H
