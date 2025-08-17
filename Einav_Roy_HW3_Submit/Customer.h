#pragma once

#include "ShoppingCart.h"
#include "Vtable.h"

#define CUSTOMER_ID_LENGTH 9
#define MAX_STR_LEN 255
#define NAMES_SEP " "

/*typedef struct {
    int (*init)(struct Customer_*);
    void (*print)(const struct Customer_*);
    float (*calculateDiscount)(const struct Customer_*, float);
} CustomerVTable;*/

typedef struct Customer_
{
	char*			id;
	char*			name;
	ShoppingCart*	pCart;
	void*			pDerivedObj;
	CustomerVTable* vtable;
}Customer;


int initCustomer(Customer* pCustomer);
void printCustomer(const Customer* pCustomer);
void freeCustomer(Customer* pCustomer);
int validateCustomerId(const char* id);
int initCustomerVTable(Customer* pCustomer);
void pay(Customer* pCustomer);
void cancelShopping(Customer* pCustomer);
void getNamePart(char* part, const char* prompt);
void upperLowerCustomerName(char* name);
char* combineFirstLast(char** parts);
int isCustomerById(const Customer* pCust, const char* id);
int isCustomer(const Customer* pCust, const char* name);
