//
// Created by Sergio Shevchenko on 27/05/2020.
//

#ifndef MPI_WORD_COUNT_WC_UTILS_H
#define MPI_WORD_COUNT_WC_UTILS_H

#include <stdio.h>
#include <stdbool.h>
#include "errors.h"
#include "linked_list.h"
#include "hash_map.h"

typedef struct WordFreq {
    LinkedList *word_list;
    HashTable *word_frequencies;
} WordFreq;

typedef struct WordFreqContig {
    char *words;
    size_t word_len;
    int *frequencies;
    size_t frequencies_len;
} WordFreqContig;

/**
 * Will group file_list into number of groups using ApproxLoadDec strategy with T < 3/4(T*) guarantee
 * @param file_list LinkedList of files (contains char*)
 * @param groups    Groups file_list will be devided to
 * @param status    Overall status
 * @return          Array of LinkedLists long groups
 */
LinkedList **split_files_equally(LinkedList *file_list, unsigned int groups, enum wc_error *status);

WordFreq *word_frequencies(WordFreq *update_freq, const char *filepath, enum wc_error *status);

WordFreqContig *wc_dump(WordFreq *frequencies, enum wc_error *status);
void merge_locally(WordFreq *local_frequency, WordFreqContig to_merge_freqs);
void print_frequencies(WordFreq *frequncies, bool only_total);

#endif //MPI_WORD_COUNT_WC_UTILS_H
