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

int int_compare(const void *a, const void *b) {
    int int_a = *((int *) a);
    int int_b = *((int *) b);

    if (int_a == int_b) return 0;
    else if (int_a < int_b) return -1;
    else return 1;
}

int find_min_index(const int *in_arr, int count) {
    int min_pos = 0;
    for (int i = 0; i < count; i++) {
        if (in_arr[min_pos] > in_arr[i]) {
            min_pos = i;
        }
    }
    return min_pos;
}

/*
 * Look into header for description
 */
struct LinkedList **split_files_equally(struct LinkedList *file_list, unsigned int groups, enum wc_error *status) {
    log_debug("split_files_equally [file_list_count=%d, groups=%d] starting", ll_size(file_list), groups);

    int *workload_sep_size = calloc(groups, sizeof(int));
    int *all_files_sizes = calloc(ll_size(file_list), sizeof(int));
    struct LinkedList **splitted_files = malloc(sizeof(struct LinkedList *) * groups);

    for (int i = 0; i < groups; i++) {
        splitted_files[i] = ll_construct_linked_list();
    }

    struct Table *file_sizes_hash_table = ht_create_table((int) (ll_size(file_list) * 5)); // 5 magic number to be 20% full

    for (int i = 0; i < ll_size(file_list); i++) {
        const char *filename = (const char *) ll_find(file_list, (unsigned int) i)->data;
        int size = (int) file_size(filename, status);
        all_files_sizes[i] = size;
        ht_insert(file_sizes_hash_table, size, (void *) filename);
    }

    qsort(all_files_sizes, ll_size(file_list) + 1, sizeof(int), int_compare);
    for (int i = (int) ll_size(file_list); i > 0; i--) {
        int min_index = find_min_index(workload_sep_size, groups);
        workload_sep_size[min_index] = workload_sep_size[min_index] + all_files_sizes[i];
        ll_add_last(splitted_files[min_index], ht_lookup(file_sizes_hash_table, all_files_sizes[i]));
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
