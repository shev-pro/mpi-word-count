//
// Created by Sergio Shevchenko on 26/05/2020.
//

#ifndef MPI_WORD_COUNT_HASH_MAP_H
#define MPI_WORD_COUNT_HASH_MAP_H

#include <stdio.h>
#include <stdlib.h>

struct node {
    int key;
    void *val;
    struct node *next;
};

struct Table {
    int size;
    struct node **list;
};

static struct Table *ht_create_table(int size) {
    struct Table *t = (struct Table *) malloc(sizeof(struct Table));
    t->size = size;
    t->list = (struct node **) malloc(sizeof(struct node *) * size);
    int i;
    for (i = 0; i < size; i++)
        t->list[i] = NULL;
    return t;
}

static int ht_hash_code(struct Table *t, int key) {
    if (key < 0)
        return -(key % t->size);
    return key % t->size;
}

static void ht_insert(struct Table *t, int key, void *val) {
    int pos = ht_hash_code(t, key);
    struct node *list = t->list[pos];
    struct node *newNode = (struct node *) malloc(sizeof(struct node));
    struct node *temp = list;
    while (temp) {
        if (temp->key == key) {
            temp->val = val;
            return;
        }
        temp = temp->next;
    }
    newNode->key = key;
    newNode->val = val;
    newNode->next = list;
    t->list[pos] = newNode;
}

static void *ht_lookup(struct Table *t, int key) {
    int pos = ht_hash_code(t, key);
    struct node *list = t->list[pos];
    struct node *temp = list;
    while (temp) {
        if (temp->key == key) {
            return temp->val;
        }
        temp = temp->next;
    }
    return NULL;
}

void ht_free(struct Table *t) {
    free(t);
}

#endif //MPI_WORD_COUNT_HASH_MAP_H
