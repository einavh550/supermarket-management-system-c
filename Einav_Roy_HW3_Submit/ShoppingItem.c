#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ShoppingItem.h"

void printItemDetails(const ShoppingItem* pItem) {
    printf("Item Barcode: %s, Quantity: %d, Unit Price: %.2f\n",
        pItem->barcode, pItem->quantity, pItem->unitPrice);
}

ShoppingItem* createItemHelper(const char* barcode, float price, int quantity) {
    ShoppingItem* pItem = (ShoppingItem*)malloc(sizeof(ShoppingItem));
    if (!pItem)
        return NULL;
    strcpy(pItem->barcode, barcode);
    pItem->unitPrice = price;
    pItem->quantity = quantity;
    return pItem;
}

void freeItemHelper(void* data) {
    if (!data) return;
    free(data);  // Free allocated ShoppingItem memory
}
