#pragma once
#include "Customer.h"
#include "Product.h"
#include "ShoppingCart.h"

typedef enum { eName, eQuantity, ePrice, eNotSorted, eNumOfProducts } SortCriterion;
static const char* typeNames[eNumOfProducts] = { "ByName", "ByQuantity", "ByPrice", "NotSorted" };


typedef struct
{
    char* name;
    Customer* customerArr;
    int customerCount;
    Product** productArr;
    int productCount;
    SortCriterion  Type;
} SuperMarket;

// Initialization and management functions
int initSuperMarket(SuperMarket* pMarket);
void printSuperMarket(const SuperMarket* pMarket);
int addProduct(SuperMarket* pMarket);
int addNewProduct(SuperMarket* pMarket);
int addCustomer(SuperMarket* pMarket);
//helpers for addCustomer:
char getClubStatus();
int setupCustomerByMembership(Customer* pCustomer, char clubStatus);
int validateUniqueCustomerID(SuperMarket* pMarket, Customer* pCustomer, char clubStatus);
int appendCustomerToMarket(SuperMarket* pMarket, Customer* pCustomer);
int cloneCustomer(Customer* target, const Customer* source);


int isCustomerInMarket(SuperMarket* pMarket, Customer* pCust);
int doShopping(SuperMarket* pMarket);
Customer* doPrintCart(SuperMarket* pMarket);
int manageShoppingCart(SuperMarket* pMarket);
Customer* getCustomerShopPay(SuperMarket* pMarket);
int fillCart(SuperMarket* pMarket, ShoppingCart* pCart);
void clearCart(SuperMarket* pMarket, Customer* pCustomer);
float calculateTotalPrice(const ShoppingCart* pCart);

// Printing functions
void printProductByType(SuperMarket* pMarket);
void printAllProducts(const SuperMarket* pMarket);
void printAllCustomers(const SuperMarket* pMarket);

// Product-related functions
int isBarcodeUnique(const SuperMarket* pMarket, const char* barcode);
int getProductIndexByBarcode(SuperMarket* pMarket, const char* barcode);
Product* getProductByBarcode(SuperMarket* pMarket, const char* barcode);
Product* getProductFromUser(SuperMarket* pMarket, char* barcode);
void getUniquBarcode(char* barcode, SuperMarket* pMarket);
Product* getProductAndCount(SuperMarket* pMarket, int* pCount);

// Customer-related functions
int isCustomerIdUnique(const SuperMarket* pMarket, const char* id);
Customer* FindCustomerById(SuperMarket* pMarket, const char* id);
Customer* getCustomerWhoShop(SuperMarket* pMarket);

// Memory management functions
void freeMarket(SuperMarket* pMarket);
//void freeProducts(SuperMarket* pMarket);
//void freeCustomers(SuperMarket* pMarket);

// functions for sort and search
void sortByType(SuperMarket* pMarket);
int compareByPrice(const void* a, const void* b);
int compareByQuantity(const void* a, const void* b);
int compareByName(const void* a, const void* b);

void searchByType(SuperMarket* pMarket);
int getProductSearchKey(int type, Product* key);
int (*getComparisonFunction(int type))(const void*, const void*);
int isSorted(Product** productArr, int size, int (*compareFunc)(const void*, const void*));

// implementing binary and text files
void saveBinInfo(SuperMarket* pMarket);
void saveTXTinfo(SuperMarket* pMarket);

int loadTXTinfo(SuperMarket* pMarket);
int loadCustomersFromFile(FILE* filePtr, SuperMarket* pMarket);
int loadSingleCustomer(FILE* filePtr, SuperMarket* pMarket);
int readCustomerInfo(FILE* filePtr, Customer* customer);
int extractCustomerData(FILE* filePtr, Customer* customer, char* clubStatus);
int getCustomerName(FILE* filePtr, char* firstName, char* lastName);
int getCustomerID(FILE* filePtr, char* id, char* clubStatus);
int assignCustomerDetails(Customer* customer, const char* firstName, const char* lastName, const char* id);
int setupClubMemberFromFile(FILE* filePtr, Customer* customer);

int loadBinInfo(SuperMarket* pMarket);
int processSuperMarketData(FILE* filePtr, SuperMarket* pMarket);
int loadMarketName(FILE* filePtr, char** marketName);
int loadProducts(FILE* filePtr, Product*** productList, int* productCount);
int fetchProductDetails(FILE* filePtr, Product** productList, int count);
int retrieveProduct(FILE* filePtr, Product** productPtr);
void freeProductList(Product** productList, int count);



// general function
void generalArrayFunction(void* array, int elementCount, size_t elementSize, void (*func)(void*));