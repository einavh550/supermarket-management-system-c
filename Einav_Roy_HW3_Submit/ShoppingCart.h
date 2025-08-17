#pragma once
#include "ShoppingItem.h"
#include "list.h"
#include "ShoppingCart.h"

typedef struct {
    LIST itemsList;
} ShoppingCart;

void initCart(ShoppingCart* pCart);
float computeTotalCost(const ShoppingCart* pCart);
int addItemToCart(ShoppingCart* pCart, const char* barcode, float price, int quantity);
void printShoppingCart(const ShoppingCart* pCart);
ShoppingItem* GetItemByBarcode(const ShoppingCart* pCart, const char* barcode);
void freeShoppingCart(ShoppingCart* pCart);
void calculateAndPrintFinalPrice(const ShoppingCart* pCart);