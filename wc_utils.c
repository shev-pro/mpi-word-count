//
// Created by Sergio Shevchenko on 27/05/2020.
//

#include "wc_utils.h"
#include <stdio.h>
#include <string.h>
#include "linked_list.h"
#include "hash_map.h"
#include "fs_utils.h"
#include "log.h"
#include "wc_constants.h"

int int_compare(const void *a, const void *b) {
    int int_a = *((int *) a);
    int int_b = *((int *) b);

    if (int_a == int_b) return 0;
    else if (int_a < int_b) return -1;
    else return 1;
}

int find_min_index(const long *in_arr, int count) {
    int min_pos = 0;
    for (int i = 0; i < count; i++) {
        if (in_arr[min_pos] > in_arr[i]) {
            min_pos = i;
        }
    }
    return min_pos;
}

/**
 * Will group file_list into number of groups using ApproxLoadDec strategy with T < 3/4(T*) guarantee
 * @param file_list LinkedList of files (contains char*)
 * @param groups    Groups file_list will be devided to
 * @param status    Overall status
 * @return          Array of LinkedLists long groups
 */
struct LinkedList **split_files_equally(struct LinkedList *file_list, unsigned int groups, enum wc_error *status) {
    log_debug("split_files_equally [file_list_count=%d, groups=%d] starting", ll_size(file_list), groups);

    long *workload_sep_size = calloc(groups, sizeof(long));
    int *all_files_sizes = calloc(ll_size(file_list), sizeof(int));
    struct LinkedList **splitted_files = malloc(sizeof(struct LinkedList *) * groups);
    if (NULL == splitted_files) {
        *status = OOM_ERROR;
        return NULL;
    }

    for (int i = 0; i < groups; i++) {
        splitted_files[i] = ll_construct_linked_list();
        if (NULL == splitted_files[i]) {
            *status = OOM_ERROR;
            return NULL;
        }

    }

    struct HashTable *file_sizes_hash_table = ht_create_table(
            (int) (ll_size(file_list) * 5)); // 5 magic number to be 20% full
    if (NULL == file_sizes_hash_table) {
        free(splitted_files);
        *status = OOM_ERROR;
        return NULL;
    }

    for (int i = 0; i < ll_size(file_list); i++) {
        const char *filename = (const char *) ll_find(file_list,
                                                      (unsigned int) i)->data; // change ll_find with sthg performant
        int size = (int) file_size(filename, status);
        all_files_sizes[i] = size;
        ht_insert_int(file_sizes_hash_table, size, (void *) filename);
    }
    qsort(all_files_sizes, ll_size(file_list), sizeof(int), int_compare);

    for (int i = ((int) ll_size(file_list) - 1); i > 0; i--) {
        int min_index = find_min_index(workload_sep_size, groups);
        workload_sep_size[min_index] = workload_sep_size[min_index] + all_files_sizes[i];
        char *filename = ht_lookup(file_sizes_hash_table, all_files_sizes[i]);

        ll_add_last(splitted_files[min_index], filename);
    }

    for (int i = 0; i < groups; i++) {
        log_debug("split_files_equally [file_list_count=%d, groups=%d] group_no=%d, file_count=%d, file_size=%d",
                  ll_size(file_list), groups, i, ll_size(splitted_files[i]), workload_sep_size[i]);
    }

    ht_free(file_sizes_hash_table);
    free(workload_sep_size);
    free(all_files_sizes);

    return splitted_files;
}

void print_frequencies(struct WordFreq *frequncies) {
    struct LinkedList *word_list = frequncies->word_list;
    struct HashTable *word_freq = frequncies->word_frequencies;

    struct Node *current = ll_next(word_list, NULL);
    long words = 0;
    while (NULL != current) {
        int word_count = *(int *) ht_lookup_str(word_freq, (char *) current->data);
        log_debug("%d => %s", word_count, (char *) current->data);
        words = word_count + words;
        current = ll_next(word_list, current);
    }
    log_debug("Total words: %li", words);
}

enum wc_error dump(struct WordFreq *frequncies, char *serialized, long *size) {
    struct LinkedList *word_list = frequncies->word_list;
    struct HashTable *word_freq = frequncies->word_frequencies;


}

struct WordFreq *word_frequencies(const char *filepath, enum wc_error *status) {
    log_debug("word_frequencies [filepath=%s]", filepath);

    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        *status = IO_ERROR;
        return NULL;
    }

    struct LinkedList *word_list = ll_construct_linked_list();
    struct HashTable *frequencies = ht_create_table(500); //todo define this value using something better

    if (NULL == word_list || NULL == frequencies) {
        *status = OOM_ERROR;
        return NULL;
    }

    char c;
    char buf[MAX_WORD_SIZE];
    int buf_pos = 0;
    while ((c = (char) fgetc(fp)) != EOF) {
        if (c == ' ' || c == '\n' || c == '\t') {
            buf[buf_pos] = '\0';
            buf_pos = 0;
            if (strlen(buf) > 0) {
                int *count = ht_lookup_str(frequencies, buf);
                if (NULL == count) { // First time seeing this word
                    count = calloc(1, sizeof(int));
                    char *word = calloc(strlen(buf) + 1, sizeof(char));
                    strncpy(word, buf, MAX_WORD_SIZE);
                    *count = 1;
                    ht_insert_str(frequencies, buf, count);
                    ll_add_last(word_list, word);
                } else {
                    *count = *count + 1;
                }
            }
        } else {
            buf[buf_pos] = c;
            buf_pos++;
        }
    }
    fclose(fp);

    struct WordFreq *result = calloc(1, sizeof(struct WordFreq));
    result->word_list = word_list;
    result->word_frequencies = frequencies;

    return result;
}
