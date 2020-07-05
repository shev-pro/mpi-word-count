//
// Created by Sergio Shevchenko on 27/05/2020.
//

#include "wc_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
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
LinkedList **
split_files_equally(LinkedList *file_list, unsigned int groups, unsigned int *actual_workers, enum wc_error *status) {
    log_debug("split_files_equally [file_list_count=%d, groups=%d] starting", ll_size(file_list), groups);

    long *workload_sep_size = calloc(groups, sizeof(long));
    int *all_files_sizes = calloc(ll_size(file_list), sizeof(int));
    LinkedList **splitted_files = malloc(sizeof(LinkedList *) * groups);
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

    HashTable *file_sizes_hash_table = ht_create_table(
            (int) (ll_size(file_list) * 5)); // 5 magic number to be 20% full
    if (NULL == file_sizes_hash_table) {
//        free(splitted_files);
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
    for (int i = ((int) ll_size(file_list) - 1); i >= 0; i--) {
        int min_index = find_min_index(workload_sep_size, groups);
        workload_sep_size[min_index] = workload_sep_size[min_index] + all_files_sizes[i]; //TODO same size file problem
        char *filename = ht_lookup(file_sizes_hash_table, all_files_sizes[i]);

        ll_add_last(splitted_files[min_index], filename);
    }

    *actual_workers = 0;
    for (int i = 0; i < groups; i++) {
        log_debug("split_files_equally [file_list_count=%d, groups=%d] group_no=%d, file_count=%d, file_size=%d",
                  ll_size(file_list), groups, i, ll_size(splitted_files[i]), workload_sep_size[i]);
        if (ll_size(splitted_files[i]) > 0) {
            *actual_workers = *actual_workers + 1;
        }
    }
    log_debug("split_files_equally actual_workers %d", *actual_workers);

    ht_free(file_sizes_hash_table); //TODO fix this
    free(workload_sep_size);
    free(all_files_sizes);

    return splitted_files;
}

void print_frequencies(WordFreq *frequncies, bool only_total) {
    LinkedList *word_list = frequncies->word_list;
    HashTable *word_freq = frequncies->word_frequencies;

    struct Node *current = ll_next(word_list, NULL);
    long words = 0;
    while (NULL != current) {
        int word_count = *(int *) ht_lookup_str(word_freq, (char *) current->data);
        if (only_total == false) {
            log_debug("%d => %s", word_count, (char *) current->data);
        }
        words = word_count + words;
        current = ll_next(word_list, current);
    }
    log_debug("Total words: %li", words);
}

WordFreqContig *wc_dump(WordFreq *frequencies, enum wc_error *status) {
    LinkedList *word_list = frequencies->word_list;
    HashTable *word_freq = frequencies->word_frequencies;
    *status = NO_ERROR;

    WordFreqContig *res = calloc(sizeof(WordFreqContig), 1);

    res->frequencies_len = ll_size(word_list);
    int *local_frequency_array = calloc(res->frequencies_len, sizeof(int *));
    if (NULL == local_frequency_array) {
        *status = OOM_ERROR;
        return NULL;
    }

    struct Node *current = ll_next(word_list, NULL);
    int freq_pos = 0;
    while (NULL != current) {
        void *frequency_val = ht_lookup_str(word_freq, current->data);
        if (frequency_val == NULL) {
            log_error("Key not found %s hash map error?", current->data);
            continue;
        }
        local_frequency_array[freq_pos++] = *(int *) frequency_val;
//        printf("%d\n", local_frequency_array[freq_pos - 1]);
        current = ll_next(word_list, current);
    }

    res->frequencies = local_frequency_array;
    res->words = ll_join(word_list, '|', &res->word_len);
    if (NULL == res->words) {
        log_error("Words is null, OOM Error?");
        *status = GENERIC_ERROR;
    }

    return res;
}

WordFreq *word_frequencies(WordFreq *update_freq, const char *filepath, enum wc_error *status) {
    log_debug("word_frequencies [filepath=%s]", filepath);

    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        *status = IO_ERROR;
        return NULL;
    }
    LinkedList *word_list;
    HashTable *frequencies;

    if (NULL == update_freq) {
        word_list = ll_construct_linked_list();
        frequencies = ht_create_table(500); //todo define this value using something better should be ± 20% of capacity
    } else {
        word_list = update_freq->word_list;
        frequencies = update_freq->word_frequencies;
    }

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
                    if (NULL == word) {
                        *status = OOM_ERROR;
                        return NULL;
                    }
                    strcpy(word, buf);
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
    if (update_freq == NULL) {
        update_freq = calloc(1, sizeof(WordFreq));
    }

    update_freq->word_list = word_list;
    update_freq->word_frequencies = frequencies;
    return update_freq;
}

void merge_locally(WordFreq *local_frequency, WordFreqContig to_merge_freqs) { // todo handle errors
    log_debug("merge_locally [local_freq_len=%d, to_merge_freq=%d]", ll_size(local_frequency->word_list),
              to_merge_freqs.word_len);
    LinkedList *to_merge_words = ll_construct_linked_list();
    HashTable *local_hm = local_frequency->word_frequencies;
    ll_split(to_merge_words, to_merge_freqs.words, '|');
    struct Node *current = ll_next(to_merge_words, NULL);
    int pos = 0;
    while (NULL != current) {
        int *local_value = (int *) ht_lookup_str(local_hm, (char *) current->data);

        if (NULL == local_value) {
            int *count = malloc(sizeof(int)); // must reallocate value to free all freq array nextly
            *count = to_merge_freqs.frequencies[pos];
            ht_insert_str(local_hm, (char *) current->data, count);
//            ll_add_last(local_frequency->word_list, (char *) current->data);
            current = ll_next(to_merge_words, current);
        } else {
            *local_value = *local_value + to_merge_freqs.frequencies[pos];
            struct Node *to_remove = current;
            current = ll_next(to_merge_words, current);
            ll_remove_node(to_merge_words, to_remove, true);
        }
        pos++;
    }

    local_frequency->word_list = ll_merge(local_frequency->word_list, to_merge_words);
//    ll_print(local_frequency->word_list);
}