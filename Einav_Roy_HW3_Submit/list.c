/******/
/*   list.c   */
/******/

#include <stdio.h>
#include <stdlib.h>
#include "list.h"


//////////////////////////////////////////
// Init
// Aim:        create new list
// Input:    pointer to the list structure
// Output:    TRUE if succeeded
//////////////////////////////////////////
BOOL L_init(LIST* pList)
{
    if (pList == NULL)
        return False;    // no list to initialize

    pList->head.next = NULL;
    return True;
}

/////////////////////////////////////////////////////////////////
// Insert
// Aim:        add new node
// Input:    pointer to the node BEFORE the place for the new one
//            a value to be stored in the new node
// Output:    pointer to the new node
/////////////////////////////////////////////////////////////////
NODE* L_insert(NODE* pNode, void* data)
{
    NODE* tmp;
    if (!pNode)
        return NULL;
    tmp = (NODE*)malloc(sizeof(NODE));    // new node
    if (tmp != NULL) {
        tmp->key = data;
        tmp->next = pNode->next;
        pNode->next = tmp;
    }
    return tmp;
}

//////////////////////////////////////////////////////////////
// Delete
// Aim:        erase node
// Input:    pointer to the node BEFORE the node to be deleted 
// Output:    TRUE if succeeded
//////////////////////////////////////////////////////////////
BOOL L_delete(NODE* pNode)
{
    NODE* tmp;
    if (!pNode)
        return False;
    tmp = pNode->next;
    if (!tmp)
        return False;
    freeData(tmp->key);  // Using helper function
    pNode->next = tmp->next;
    free(tmp);
    return True;
}

/////////////////////////////////////////////////////////
// Find
// Aim:        search for a value
// Input:    pointer to the node to start with 
//            a value to be found
// Output:    pointer to the node containing the Value
/////////////////////////////////////////////////////////
const NODE* L_find(const NODE* pNode, const void* data, int (compareData)(const void*, const void*))
{
    if (!pNode)
        return NULL;
    const NODE* current = pNode->next;
    while (current) {
        if (compareData(current->key, data) == 0) {  // Using helper function
            return current;
        }
        current = current->next;
    }
    return NULL;
}

////////////////////////////////////////////////
// Free (additional function)
// Aim:        free the list memory
// Input:    pointer to the list structure
// Output:    TRUE if succeeded
////////////////////////////////////////////////
BOOL L_free(LIST* pList, void (freeData)(void*l))
{
    if (!pList)
        return False;
    NODE* current = pList->head.next;
    while (current) {
        NODE* next = current->next;
        if (freeData)
            freeData(current->key);
        free(current);
        current = next;
    }
    pList->head.next = NULL;
    return True;
}

////////////////////////////////////////////////
// Print (additional function)
// Aim:        print the list content (assume the DATA is int)
// Input:    pointer to the list structure
// Output:    a number of the printed elements
////////////////////////////////////////////////
int L_print(const LIST* pList, void (printFunc)(const void*))
{
    if (!pList || !printFunc)
        return 0;
    int c = 0;
    const NODE* current = pList->head.next;
    while (current) {
        printFunc(current->key);
        c++;
        current = current->next;
    }
    return c;
}



// Helper functions implementation
void freeData(void* pData) {
    if (pData)
        free(pData);
}

int compareData(const void* a, const void* b) {
    if (!a || !b)
        return -1;
    return ((int)a - (int)b);
}