#pragma once


#ifndef _LIST_
#define _LIST_

#include "def.h"

typedef enum { FALSE, TRUE } BOOL;

/*typedef struct node {
    void* data;
    struct node* next;
} NODE;*/

typedef struct {
    NODE head;
} LIST;

BOOL L_init(LIST* pList);
NODE* L_insert(NODE* pNode, void* pData);
BOOL L_free(LIST* pList, void (*freeFunc)(void*));
BOOL L_delete(LIST* pList, void* key, int (*compareFunc)(const void*, const void*));
const NODE* L_find(const NODE* pNode, const void* data, int (compareData)(const void*, const void*));
int L_print(const LIST* pList, void (printFunc)(const void*));
void freeData(void* pData);
int compareData(const void* a, const void* b);
#endif