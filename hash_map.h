//
// Created by Sergio Shevchenko on 26/05/2020.
//

#ifndef MPI_WORD_COUNT_HASH_MAP_H
#define MPI_WORD_COUNT_HASH_MAP_H

#include <stdio.h>
#include <stdlib.h>
#include "hash_map_crc.h"

struct node {
    int key;
    void *val;
    struct node *next;
};

struct HashTable {
    int size;
    struct node **list;
};

static struct HashTable *ht_create_table(int size) {
    struct HashTable *t = (struct HashTable *) malloc(sizeof(struct HashTable));
    if (NULL == t)
        return NULL;
    t->size = size;
    t->list = (struct node **) malloc(sizeof(struct node *) * size);
    int i;
    for (i = 0; i < size; i++)
        t->list[i] = NULL;
    return t;
}

static int ht_hash_code(struct HashTable *t, int key) {
    if (key < 0)
        return -(key % t->size);
    return key % t->size;
}


static void ht_insert_int(struct HashTable *t, int key, void *val) {
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

static void ht_insert_str(struct HashTable *t, const char *key, void *val) {
    int int_key = hashmap_hash_int(key);
    ht_insert_int(t, int_key, val);
}

static void *ht_lookup(struct HashTable *t, int key) {
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

static void *ht_lookup_str(struct HashTable *t, const char *key) {
    int int_key = hashmap_hash_int(key);
//    printf("Key %d\n", int_key);
    return ht_lookup(t, int_key);
}

static void ht_free(struct HashTable *t) {
    free(t);
}

#endif //MPI_WORD_COUNT_HASH_MAP_H
