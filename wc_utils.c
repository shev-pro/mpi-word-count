//
// Created by Sergio Shevchenko on 27/05/2020.
//

#include "wc_utils.h"
#include <stdio.h>
#include <string.h>
#include "linked_list.h"
#include "hash_map.h"
#include "fs_utils.h"

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

struct LinkedList **split_files_equally(struct LinkedList *file_list, unsigned int groups, enum wc_error *status) {
    int *workload_sep_size = calloc(groups, sizeof(int));
    int *files_sizes_arr = calloc(ll_size(file_list), sizeof(int));
    struct LinkedList **splitted_files = malloc(sizeof(struct LinkedList *) * groups);

    for (int i = 0; i < groups; i++) {
        splitted_files[i] = ll_construct_linked_list();
    }

    struct Table *file_sizes = ht_create_table((int) (ll_size(file_list) * 5)); // to be 20% full
    ll_print(file_list);

    for (int i = 0; i < ll_size(file_list); i++) {
        const char *filename = (const char *) ll_find(file_list, (unsigned int) i)->data;
        int size = (int) file_size(filename, status);
        files_sizes_arr[i] = size;
        ht_insert(file_sizes, size, (void *) filename);
    }

    qsort(files_sizes_arr, ll_size(file_list) + 1, sizeof(int), int_compare);
    for (int i = (int) ll_size(file_list); i > 0; i--) {
        int min_index = find_min_index(workload_sep_size, groups);
        workload_sep_size[min_index] = workload_sep_size[min_index] + files_sizes_arr[i];
        ll_add_last(splitted_files[min_index], ht_lookup(file_sizes, files_sizes_arr[i]));
    }

    for (int i = 0; i < groups; i++) {
        printf("Group %d with total size %d\n", i, workload_sep_size[i]);
        ll_print(splitted_files[i]);
    }

    return splitted_files;
}
