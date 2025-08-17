#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ShoppingCart.h"
#include "General.h"

void initCart(ShoppingCart* pCart) {
    if (!pCart) return;
    L_init(&pCart->itemsList);
}

float computeTotalCost(const ShoppingCart* pCart) {
    if (!pCart) return 0;
    float totalCost = 0;
    for (NODE* node = pCart->itemsList.head.next; node != NULL; node = node->next) {
        ShoppingItem* item = (ShoppingItem*)node->key;
        totalCost += item->unitPrice * item->quantity;
    }
    return totalCost;
}

int addItemToCart(ShoppingCart* pCart, const char* barcode, float price, int quantity) {
    if (!pCart || !barcode || quantity <= 0) return 0;

    NODE* previousNode = &pCart->itemsList.head;
    NODE* currentNode = previousNode->next;

    while (currentNode) {
        ShoppingItem* item = (ShoppingItem*)currentNode->key;
        int comparison = strcmp(barcode, item->barcode);
        if (comparison == 0) {
            item->quantity += quantity;
            return 1;
        }
        else if (comparison < 0) {
            break;
        }
        previousNode = currentNode;
        currentNode = currentNode->next;
    }

    ShoppingItem* newItem = createItemHelper(barcode, price, quantity);
    if (!newItem) return 0;

    if (!L_insert(previousNode, newItem)) {
        free(newItem);
        return 0;
    }
    return 1;
}

void printShoppingCart(const ShoppingCart* pCart) {
    if (!pCart) return;
    for (NODE* currentNode = pCart->itemsList.head.next; currentNode != NULL; currentNode = currentNode->next) {
        printItemDetails((ShoppingItem*)currentNode->key);
    }
}

ShoppingItem* GetItemByBarcode(const ShoppingCart* pCart, const char* barcode) {
    if (!pCart || !barcode) return NULL;
    for (NODE* node = pCart->itemsList.head.next; node != NULL; node = node->next) {
        ShoppingItem* item = (ShoppingItem*)node->key;
        if (strcmp(item->barcode, barcode) == 0) {
            return item;
        }
    }
    return NULL;
}

void freeShoppingCart(ShoppingCart* pCart) {
    if (!pCart) return;
    L_free(&pCart->itemsList, freeItemHelper);
}

void calculateAndPrintFinalPrice(const ShoppingCart* pCart) {
    if (!pCart || pCart->itemsList.head.next == NULL) {
        printf("Shopping cart is empty!\n");
        return;
    }

    float totalCost = 0.0;
    int itemCount = 0;

    for (NODE* currentNode = pCart->itemsList.head.next; currentNode != NULL; currentNode = currentNode->next) {
        ShoppingItem* item = (ShoppingItem*)currentNode->key;
        float itemTotal = item->unitPrice * item->quantity;
        totalCost += itemTotal;
        itemCount += item->quantity;
    }
}
