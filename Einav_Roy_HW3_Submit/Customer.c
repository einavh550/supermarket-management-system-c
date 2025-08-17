#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Customer.h"
#include "General.h"
#include "ClubMember.h"

// Function prototype added for personal usage- checking error
float calculateCustomerDiscount(const Customer* pCustomer, float totalPrice);

int initCustomer(Customer* pCustomer)
{
    initCustomerVTable(pCustomer);
    if (!pCustomer->vtable) return 0;

    char firstName[MAX_STR_LEN];
    char lastName[MAX_STR_LEN];

    char* parts[2] = { firstName, lastName };
    char* msgParts[2] = { "Enter first name: ", "Enter last name: " };

    for (int i = 0; i < 2; i++) {
        getNamePart(parts[i], msgParts[i]);
        upperLowerCustomerName(parts[i]);
    }

    char* fullName = combineFirstLast(parts);
    if (!fullName)
        return 0;

    pCustomer->name = fullName;

    char msg[MAX_STR_LEN];
    sprintf(msg, "ID should be %d digits (e.g., 123456789): ", CUSTOMER_ID_LENGTH);
    do {
        pCustomer->id = getStrExactLength(msg);
        if (!pCustomer->id)
            return 0;
    } while (!validateCustomerId(pCustomer->id));

    pCustomer->pCart = NULL;
    pCustomer->pDerivedObj = NULL;
    return 1;
}

int initCustomerVTable(Customer* pCustomer) {
    pCustomer->vtable = (CustomerVTable*)malloc(sizeof(CustomerVTable));
    if (!pCustomer->vtable) {
        printf("Memory allocation failed for vtable.\n");
        return 0;
    }

    pCustomer->vtable->init = initCustomer;
    pCustomer->vtable->print = printCustomer;
    pCustomer->vtable->calculateDiscount = calculateCustomerDiscount;

    return 1;
}

float calculateCustomerDiscount(const Customer* pCustomer, float totalPrice) {
    printf("No discount applied. This customer is not a club member.\n");
    return totalPrice;
}

void getNamePart(char* part, const char* prompt)
{
    int valid = 0;
    while (!valid) {
        getsStrFixSize(part, MAX_STR_LEN, prompt);
        if (checkEmptyString(part)) {
            printf("Name cannot be empty.\n");
        }
        else if (!checkAlphaSpaceStr(part)) {
            printf("Name should contain only letters.\n");
        }
        else {
            valid = 1;
        }
    }
}

void upperLowerCustomerName(char* name)
{
    toLowerStr(name);
    while (isspace(*name))
        name++;
    *name = toupper(*name);
}

char* combineFirstLast(char** parts)
{
    char tempBuffer[MAX_STR_LEN * 2] = { 0 };
    char** nameTokens = NULL;
    int count = 0;
    size_t totalLength = 0;

    for (int i = 0; i < 2; i++) {
        nameTokens = splitCharsToWords(parts[i], &count, NAMES_SEP, &totalLength);
        if (!nameTokens)
            return NULL;

        for (int j = 0; j < count; j++) {
            strcat(tempBuffer, nameTokens[j]);
            strcat(tempBuffer, " ");
            free(nameTokens[j]);
        }
        free(nameTokens);
        nameTokens = NULL;

        if (i == 0)
            strcat(tempBuffer, "- ");
    }
    tempBuffer[strlen(tempBuffer) - 1] = '\0';

    return myStrDup(tempBuffer);
}

void printCustomer(const Customer* pCustomer)
{
    printf("Customer Name: %s\n", pCustomer->name);
    printf("Customer ID: %s\n", pCustomer->id);

    if (pCustomer->pDerivedObj) {
        ClubMember* member = (ClubMember*)pCustomer->pDerivedObj;
        printf("Club Member for %d months.\n", member->totalMonths);
    }

    if (pCustomer->pCart == NULL)
        printf("No items in shopping cart.\n");
    else
        printf("Shopping in progress.\n");
}

int validateCustomerId(const char* id)
{
    if (strlen(id) != CUSTOMER_ID_LENGTH)
        return 0;
    for (int i = 0; i < CUSTOMER_ID_LENGTH; i++) {
        if (!isdigit(id[i]))
            return 0;
    }
    return 1;
}

void pay(Customer* pCustomer)
{
    if (!pCustomer->pCart)
        return;
    printf("------ Payment Summary for %s ------\n", pCustomer->name);
    printShoppingCart(pCustomer->pCart);
    calculateAndPrintFinalPrice(pCustomer);
    printf("Payment successful.\n");
    freeShoppingCart(pCustomer->pCart);
    free(pCustomer->pCart);
    pCustomer->pCart = NULL;
}

void cancelShopping(Customer* pCustomer)
{
    if (!pCustomer->pCart)
        return;
    printf("!!! --- Purchase was canceled!!!! --- \n");
    freeShoppingCart(pCustomer->pCart);
    free(pCustomer->pCart);
    pCustomer->pCart = NULL;
}

int isCustomerById(const Customer* pCust, const char* id)
{
    return strcmp(pCust->id, id) == 0 ? 1 : 0;
}

int isCustomer(const Customer* pCust, const char* name)
{
    return strcmp(pCust->name, name) == 0 ? 1 : 0;
}

void freeCustomer(Customer* pCust)
{
    if (pCust->pCart)
        pay(pCust);
    free(pCust->name);
    free(pCust->id);
    if (pCust->pDerivedObj)
        free(pCust->pDerivedObj);
    free(pCust->vtable);
}
