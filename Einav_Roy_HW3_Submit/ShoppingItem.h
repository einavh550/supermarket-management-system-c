#pragma once
#include "Product.h"

#define BARCODE_LENGTH 10

typedef struct {
    char barcode[BARCODE_LENGTH + 1];
    float unitPrice;
    int quantity;
} ShoppingItem;

void printItemDetails(const ShoppingItem* pItem);
ShoppingItem* createItemHelper(const char* barcode, float price, int quantity);
void freeItemHelper(void* data);
