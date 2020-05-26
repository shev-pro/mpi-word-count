//
// Created by Sergio Shevchenko on 26/05/2020.
//

#ifndef MPI_WORD_COUNT_LINKED_LIST_H
#define MPI_WORD_COUNT_LINKED_LIST_H

#include <stdlib.h>
#include <stdio.h>

struct Node {
    void *data;
    struct Node *next, *prev;
};

struct LinkedList {
    struct Node *root, *tail;
    size_t _size;
};

struct Node *ll_construct_node(void *data);

struct LinkedList *ll_construct_linked_list();

static struct Node *ll_find(struct LinkedList *list, unsigned int index);

void ll_add(struct LinkedList *list, void *data);

void ll_add_last(struct LinkedList *list, void *data);

int ll_add_at(struct LinkedList *list, unsigned int index, void *data);

void ll_clear(struct LinkedList *list);

void ll_remove_first(struct LinkedList *list);

void ll_remove_last(struct LinkedList *list);

int ll_remove_at(struct LinkedList *list, unsigned int index);

int ll_remove(struct LinkedList *list, void *data);

void ll_print(struct LinkedList *list);

struct LinkedList *ll_clone(struct LinkedList *list);

size_t ll_size(struct LinkedList *list);

#endif //MPI_WORD_COUNT_LINKED_LIST_H
