//
// Created by Sergio Shevchenko on 26/05/2020.
//

#ifndef MPI_WORD_COUNT_HASH_MAP_H
#define MPI_WORD_COUNT_HASH_MAP_H

#include <stdio.h>
#include <stdlib.h>
#include "hash_map_crc.h"

typedef struct ht_node {
    int key;
    void *val;
    struct ht_node *next;
} ht_node;

typedef struct HashTable {
    int size;
    ht_node **list;
} HashTable;

static HashTable *ht_create_table(int size) {
    HashTable *t = (HashTable *) malloc(sizeof(HashTable));
    if (NULL == t)
        return NULL;
    t->size = size;
    t->list = (ht_node **) malloc(sizeof(ht_node *) * size);
    int i;
    for (i = 0; i < size; i++)
        t->list[i] = NULL;
    return t;
}

static int ht_hash_code(HashTable *t, int key) {
    if (key < 0)
        return -(key % t->size);
    return key % t->size;
}


static void ht_insert_int(HashTable *t, int key, void *val) {
    int pos = ht_hash_code(t, key);
    ht_node *list = t->list[pos];
    ht_node *newNode = (ht_node *) malloc(sizeof(ht_node));
    ht_node *temp = list;
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

static void ht_insert_str(HashTable *t, const char *key, void *val) {
    int int_key = hashmap_hash_int(key);
    ht_insert_int(t, int_key, val);
}

static void *ht_lookup(HashTable *t, int key) {
    int pos = ht_hash_code(t, key);
    ht_node *list = t->list[pos];
    ht_node *temp = list;
    while (temp) {
        if (temp->key == key) {
            return temp->val;
        }
        temp = temp->next;
    }
    return NULL;
}

static void *ht_lookup_str(HashTable *t, const char *key) {
    int int_key = hashmap_hash_int(key);
//    printf("Key %d\n", int_key);
    return ht_lookup(t, int_key);
}

static void ht_free(HashTable *t) {
    free(t);
}

#endif //MPI_WORD_COUNT_HASH_MAP_H
