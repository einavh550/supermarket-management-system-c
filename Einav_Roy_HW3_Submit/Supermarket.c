#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Supermarket.h"
#include "Product.h"
#include "Customer.h"
#include "General.h"
#include "ShoppingCart.h"
#include "ClubMember.h"

#define STRING_MAX_LEN 255
#define ID_LEN 10
#define SUPERMARKET_BIN_FILE "SuperMarket.bin"
#define CUSTOMERS_TXT_FILE "Customers.txt"

#define SORT_BY_NAME 1
#define SORT_BY_QUANTITY 2
#define SORT_BY_PRICE 3

int initSuperMarket(SuperMarket* pMarket) {
    pMarket->customerCount = 0;
    pMarket->customerArr = NULL;
    pMarket->productCount = 0;
    pMarket->productArr = NULL;
    pMarket->Type = eNotSorted;


    if (!loadBinInfo(pMarket)) {
        printf("Failed loading from binary file.\n");
        pMarket->name = getStrExactLength("Enter market name: ");
        if (!pMarket->name) {
            printf("Failed to allocate memory for market name.\n");
            return 0;
        }
    }

    if (!loadTXTinfo(pMarket)) {
        printf("Attention: Customer data could not be loaded. An empty list will be used.\n");
    }

    return 1;
}

void printSuperMarket(const SuperMarket* pMarket) {
    printf("Super Market Name: %s\t", pMarket->name);
    printf("\n");
    printAllProducts(pMarket);
    printf("\n");
    printAllCustomers(pMarket);
}

int	addProduct(SuperMarket* pMarket)
{
    char answer;
    char barcode[BARCODE_LENGTH + 1];
    Product* pProd = NULL;

    printf("\n");
    printf("Adding new product? y/Y: ");
    scanf("%c", &answer);
    getchar();

    if (toupper(answer) == 'Y')
    {
        if (!addNewProduct(pMarket))
        {
            free(pProd);
            return 0;
        }
        return 1;
    }
    else if (pMarket->productCount > 0)
    {
        printf("Do you want to increase the amount of an existing product? y/Y: ");
        scanf("%c", &answer);
        getchar();
        if (toupper(answer) == 'Y')
        {
            printAllProducts(pMarket);
            pProd = getProductFromUser(pMarket, barcode);
            if (pProd != NULL) //This barcode exist in stock
                updateProductCount(pProd);
        }
    }
    else
        return 0;

    return 1;
}
int addNewProduct(SuperMarket* pMarket) {
    pMarket->productArr = (Product**)realloc(pMarket->productArr, (pMarket->productCount + 1) * sizeof(Product*));
    if (!pMarket->productArr) return 0;

    Product* pProd = (Product*)calloc(1, sizeof(Product));
    if (!pProd) return 0;

    initProductNoBarcode(pProd);

    do {
        generateBarcode(pProd);
    } while (!isBarcodeUnique(pMarket, pProd->barcode));

    pMarket->productArr[pMarket->productCount] = pProd;
    pMarket->productCount++;
    return 1;
}


int addCustomer(SuperMarket* pMarket) {
    Customer newCustomer;
    char clubStatus = getClubStatus();

    if (!setupCustomerByMembership(&newCustomer, clubStatus)) {
        return 0;
    }

    if (!validateUniqueCustomerID(pMarket, &newCustomer, clubStatus)) {
        return 0;
    }

    if (!appendCustomerToMarket(pMarket, &newCustomer)) {
        return 0;
    }

    return 1;
}

char getClubStatus() {
    char clubStatus;
    printf("Is the customer a club member? (y/Y for yes): ");
    scanf(" %c", &clubStatus);
    getchar();
    return toupper(clubStatus);
}

int setupCustomerByMembership(Customer* pCustomer, char clubStatus) {
    if (clubStatus == 'Y') {
        if (!initClubMember(pCustomer)) {
            printf("Error: Failed to register club member.\n");
            return 0;
        }
    }
    else {
        if (!initCustomer(pCustomer)) {
            printf("Error: Failed to initialize customer.\n");
            return 0;
        }
    }
    return 1;
}

int validateUniqueCustomerID(SuperMarket* pMarket, Customer* pCustomer, char clubStatus) {
    while (!isCustomerIdUnique(pMarket, pCustomer->id)) {
        printf("This ID is already in use. Please try again.\n");
        freeCustomer(pCustomer);

        // Re-initialize the customer details
        if (!setupCustomerByMembership(pCustomer, clubStatus)) {
            return 0;
        }
    }
    return 1;
}

int appendCustomerToMarket(SuperMarket* pMarket, Customer* pCustomer) {
    Customer* updatedList = realloc(pMarket->customerArr, (pMarket->customerCount + 1) * sizeof(Customer));
    if (!updatedList) {
        printf("Memory allocation error.\n");
        return 0;
    }

    pMarket->customerArr = updatedList;

    Customer* addedCustomer = &pMarket->customerArr[pMarket->customerCount];
    if (!cloneCustomer(addedCustomer, pCustomer)) {
        return 0;
    }

    pMarket->customerCount++;
    return 1;
}

int cloneCustomer(Customer* target, const Customer* source) {
    target->name = strdup(source->name);
    target->id = strdup(source->id);

    if (!target->name || !target->id) {
        printf("Error allocating memory for customer details.\n");
        freeCustomer(target);
        return 0;
    }

    target->vtable = source->vtable;

    if (source->pDerivedObj) {
        target->pDerivedObj = malloc(sizeof(ClubMember));
        if (!target->pDerivedObj) {
            printf("Memory allocation failure for club membership.\n");
            freeCustomer(target);
            return 0;
        }
        memcpy(target->pDerivedObj, source->pDerivedObj, sizeof(ClubMember));
    }
    else {
        target->pDerivedObj = NULL;
    }

    target->pCart = NULL;
    return 1;
}


int isCustomerInMarket(SuperMarket* pMarket, Customer* pCust) {
    for (int i = 0; i < pMarket->customerCount; i++) {
        if (strcmp(pMarket->customerArr[i].id, pCust->id) == 0) {
            return 1;
        }
    }
    return 0;
}

int doShopping(SuperMarket* pMarket) {
    if (!pMarket || pMarket->customerCount == 0) {
        printf("No customers available for shopping.\n");
        return 0;
    }

    Customer* pCustomer = getCustomerShopPay(pMarket);
    if (pCustomer == NULL) {
        printf("Customer not found.\n");
        return 0;
    }

    if (pCustomer->pCart == NULL) {
        pCustomer->pCart = (ShoppingCart*)malloc(sizeof(ShoppingCart));
        if (pCustomer->pCart == NULL) {
            printf("Failed to allocate memory for cart.\n");
            return 0;
        }
        initCart(pCustomer->pCart);
    }

    int itemsAdded = fillCart(pMarket, pCustomer->pCart);
    if (itemsAdded <= 0) {
        printf("No items were added to the cart. Cancelling shopping...\n");
        free(pCustomer->pCart);
        pCustomer->pCart = NULL;
        return 0;
    }

    float totalCost = calculateTotalPrice(pCustomer->pCart);
    float discountedCost = pCustomer->vtable->calculateDiscount(pCustomer, totalCost);

    printf("\n------ Final Shopping Summary ------\n");
    printShoppingCart(pCustomer->pCart);
    printf("Original Price: %.2f\n", totalCost);
    printf("Discounted Price: %.2f\n", discountedCost);

    return 1;
}

float calculateTotalPrice(const ShoppingCart* cart) {
    if (!cart || cart->itemsList.head.next == NULL) {
        return 0.0f;  // No items in cart
    }

    float totalAmount = 0.0f;
    NODE* currentItem = cart->itemsList.head.next;

    for (; currentItem != NULL; currentItem = currentItem->next) {
        ShoppingItem* item = (ShoppingItem*)currentItem->key;
        totalAmount += item->unitPrice * item->quantity;
    }

    return totalAmount;
}


Customer* doPrintCart(SuperMarket* pMarket) {
    if (!pMarket) {
        printf("Invalid supermarket data.\n");
        return NULL;
    }

    Customer* customer = getCustomerShopPay(pMarket);
    if (customer == NULL) {
        printf("Customer not found.\n");
        return NULL;
    }

    if (customer->pCart && customer->pCart->itemsList.head.next) {
        printShoppingCart(customer->pCart);
        calculateAndPrintFinalPrice(customer->pCart);

        float totalCost = calculateTotalPrice(customer->pCart);
        float discountedCost = customer->vtable->calculateDiscount(customer, totalCost);

        printf("Original Price: %.2f\n", totalCost);
        printf("Discounted Price: %.2f\n", discountedCost);

    }
    else {
        printf("The shopping cart is currently empty.\n");
        return NULL;
    }

    return customer;
}


int manageShoppingCart(SuperMarket* pMarket) {
    if (!pMarket) {
        printf("Invalid supermarket data.\n");
        return 0;
    }

    Customer* pCustomer = doPrintCart(pMarket);
    if (pCustomer == NULL) {
        printf("No valid customer found.\n");
        return 0;
    }

    printf("Proceed to checkout? (Enter 'y' or 'Y' to confirm, any other key to cancel): ");
    char response;
    do {
        scanf(" %c", &response);
    } while (isspace(response));

    if (toupper(response) == 'Y') {
        pay(pCustomer);
    }
    else {
        cancelShopping(pCustomer);
        clearCart(pMarket, pCustomer);
    }

    return 1;
}


Customer* getCustomerShopPay(SuperMarket* pMarket) {
    if (!pMarket) {
        printf("Supermarket data is not available.\n");
        return NULL;
    }

    if (pMarket->customerCount <= 0) {
        printf("No registered customers available.\n");
        return NULL;
    }

    if (pMarket->customerCount <= 0) {
        printf("The store has no products available for purchase.\n");
        return NULL;
    }

    Customer* pCustomer = getCustomerWhoShop(pMarket);
    if (pCustomer == NULL) {
        printf("Customer not found in the system.\n");
        return NULL;
    }

    return pCustomer;
}

Customer* getCustomerWhoShop(SuperMarket* pMarket)
{
    printAllCustomers(pMarket);
    char searchTerm[MAX_STR_LEN];
    getsStrFixSize(searchTerm, sizeof(searchTerm), "Who is shopping? Enter customer id\n");

    Customer* pCustomer = FindCustomerById(pMarket, searchTerm);

    return pCustomer;
}



int fillCart(SuperMarket* pMarket, ShoppingCart* pCart) {
    if (!pMarket || !pCart) {
        printf("Invalid market or cart.\n");
        return 0;
    }

    printAllProducts(pMarket);
    char choice;
    int itemsAdded = 0;

    while (1) {
        printf("Would you like to add a product? (Enter y/Y to proceed, any other key to quit): ");
        scanf(" %c", &choice);  // Notice the space before %c to consume newline

        if (toupper(choice) != 'Y') {
            printf("Exiting shopping process.\n");
            break;
        }

        int quantity = 0;
        Product* selectedProduct = getProductAndCount(pMarket, &quantity);

        if (selectedProduct != NULL) {
            if (addItemToCart(pCart, selectedProduct->barcode, selectedProduct->price, quantity)) {
                selectedProduct->count -= quantity;  // Reduce stock
                itemsAdded += quantity;
            }
            else {
                printf("Failed to add the product to the cart.\n");
            }
        }
        else {
            printf("Product selection failed. Please try again.\n");
        }
    }

    return itemsAdded;
}


Product* getProductAndCount(SuperMarket* pMarket, int* pCount)
{
    char barcode[BARCODE_LENGTH + 1];
    Product* pProd = getProductFromUser(pMarket, barcode);
    if (pProd == NULL)
    {
        printf("No such product\n");
        return NULL;
    }

    if (pProd->count == 0)
    {
        printf("This product out of stock\n");
        return NULL;
    }

    int count;
    do {
        printf("How many items do you want? max %d\n", pProd->count);
        scanf("%d", &count);
    } while (count <= 0 || count > pProd->count);
    *pCount = count;
    return pProd;
}


void clearCart(SuperMarket* pMarket, Customer* pCustomer) {
    if (!pCustomer || !pCustomer->pCart) {
        printf("No cart to clear.\n");
        return;
    }

    NODE* itemNode = pCustomer->pCart->itemsList.head.next;

    if (!itemNode) {
        printf("Shopping cart is already empty.\n");
        return;
    }

    while (itemNode != NULL) {
        ShoppingItem* cartItem = (ShoppingItem*)itemNode->key;

        Product* foundProduct = getProductByBarcode(pMarket, cartItem->barcode);
        if (foundProduct != NULL) {
            foundProduct->count += cartItem->quantity;
        }

        itemNode = itemNode->next;
    }

    printf("Cart cleared successfully.\n");
}


void printProductByType(SuperMarket* pStore) {
    if (!pStore || pStore->productCount <= 0) {
        printf("No available products in the market.\n");
        return;
    }

    eProductType selectedType = getProductType();
    int productFound = 0;

    for (int index = 0; index < pStore->productCount; ++index) {
        if (pStore->productArr[index]->type == selectedType) {
            printProduct(pStore->productArr[index]);
            productFound++;
        }
    }

    if (!productFound) {
        printf("No products of type %s found in market: %s\n",
            getProductTypeStr(selectedType), pStore->name);
    }
}


void	printAllProducts(const SuperMarket* pMarket)
{
    printf("There are %d products\n", pMarket->productCount);
    printf("%-20s %-10s\t", "Name", "Barcode");
    printf("%-20s %-10s %-20s %-15s\n", "Type", "Price", "Count In Stoke", "Expiry Date");
    printf("-------------------------------------------------------------------------------------------------\n");

    generalArrayFunction(pMarket->productArr, pMarket->productCount, sizeof(Product*), printGeneric);
}

void	printAllCustomers(const SuperMarket* pMarket)
{
    printf("There are %d listed customers\n", pMarket->customerCount);
    for (int i = 0; i < pMarket->customerCount; i++)
        printCustomer(&pMarket->customerArr[i]);
}

int isBarcodeUnique(const SuperMarket* pMarket, const char* barcode) {
    for (int i = 0; i < pMarket->productCount; i++) {
        if (strcmp(pMarket->productArr[i]->barcode, barcode) == 0)
            return 0;
    }
    return 1;
}

int isCustomerIdUnique(const SuperMarket* pMarket, const char* id) {
    for (int i = 0; i < pMarket->customerCount; i++) {
        if (strcmp(pMarket->customerArr[i].id, id) == 0)
            return 0;
    }
    return 1;
}

int getProductIndexByBarcode(SuperMarket* pMarket, const char* barcode) {
    for (int i = 0; i < pMarket->productCount; i++) {
        if (strcmp(pMarket->productArr[i]->barcode, barcode) == 0)
            return i;
    }
    return -1;
}

Product* getProductByBarcode(SuperMarket* pMarket, const char* barcode) {
    int index = getProductIndexByBarcode(pMarket, barcode);
    return index != -1 ? pMarket->productArr[index] : NULL;
}

Product* getProductFromUser(SuperMarket* pMarket, char* barcode) {
    printf("Enter product barcode: ");
    scanf("%s", barcode);
    return getProductByBarcode(pMarket, barcode);
}

void getUniquBarcode(char* barcode, SuperMarket* pMarket) {
    do {
        printf("Enter a unique barcode: ");
        scanf("%s", barcode);
    } while (!isBarcodeUnique(pMarket, barcode));
}

Customer* FindCustomerById(SuperMarket* pMarket, const char* id) {
    for (int i = 0; i < pMarket->customerCount; i++) {
        if (strcmp(pMarket->customerArr[i].id, id) == 0)
            return &pMarket->customerArr[i];
    }
    return NULL;
}



void sortByType(SuperMarket* market) {
    printf("Choose sorting criteria for products:\n");
    printf("1. Sort by Name\n");
    printf("2. Sort by Quantity\n");
    printf("3. Sort by Price\n");

    int choice;
    if (scanf("%d", &choice) != 1) {
        printf("Invalid input. Please enter a valid number.\n");
        return;
    }

    if (choice <= 0 || choice > 3) {
        printf("Error: Selection out of range. No sorting performed.\n");
        return;
    }

    int productCount = market->productCount;

    switch (choice) {
    case SORT_BY_NAME:
        qsort(market->productArr, productCount, sizeof(Product*), compareByName);
        printf("Sorted products alphabetically by name.\n");
        break;

    case SORT_BY_QUANTITY:
        qsort(market->productArr, productCount, sizeof(Product*), compareByQuantity);
        printf("Sorted products based on available quantity.\n");
        break;

    case SORT_BY_PRICE:
        qsort(market->productArr, productCount, sizeof(Product*), compareByPrice);
        printf("Sorted products in order of price.\n");
        break;

    default:
        printf("Unexpected error occurred. Sorting aborted.\n");
        break;
    }
}

int compareByPrice(const void* a, const void* b) {
    const Product* first = *((const Product**)a);
    const Product* second = *((const Product**)b);

    float priceDifference = first->price - second->price;
    if (priceDifference > 0.0f)
        return 1;
    if (priceDifference < 0.0f)
        return -1;
    return 0;
}

int compareByQuantity(const void* a, const void* b) {
    const Product* productA = *((const Product**)a);
    const Product* productB = *((const Product**)b);

    int quantityDiff = productA->count - productB->count;
    if (quantityDiff != 0)
        return quantityDiff > 0 ? 1 : -1;
    return 0;
}

int compareByName(const void* a, const void* b) {
    const Product* productA = *((const Product**)a);
    const Product* productB = *((const Product**)b);

    int result = strcmp(productA->name, productB->name);
    if (result > 0)
        return 1;
    else if (result < 0)
        return -1;
    return 0;
}


void searchByType(SuperMarket* pMarket) {
    if (pMarket->productCount <= 0) {
        printf("No products available for search.\n");
        return;
    }

    int type;
    printf("Search by:\n1) Name\n2) Quantity\n3) Price\n");
    scanf("%d", &type);
    getchar();

    if (type < SORT_BY_NAME || type > SORT_BY_PRICE) {
        printf("Invalid selection.\n");
        return;
    }

    Product searchKey;
    if (!getProductSearchKey(type, &searchKey)) {
        printf("Invalid input.\n");
        return;
    }

    int (*compareFunc)(const void*, const void*) = getComparisonFunction(type);

    if (!isSorted(pMarket->productArr, pMarket->productCount, compareFunc)) {
        qsort(pMarket->productArr, pMarket->productCount, sizeof(Product*), compareFunc);
        printf("Products were not sorted. Sorting now.\n");
    }
    else {
        printf("Products are already sorted.\n");
    }

    Product* searchKeyPtr = &searchKey;
    Product** foundProduct = (Product**)bsearch(&searchKeyPtr, pMarket->productArr, pMarket->productCount,
        sizeof(Product*),
        compareFunc);
    if (foundProduct) {
        printf("Product found:\n");
        printProduct(*foundProduct);
    }
    else {
        printf("Product not found.\n");
    }
}

int getProductSearchKey(int type, Product* key) {
    switch (type) {
    case SORT_BY_NAME:
        printf("Enter product name: ");
        myGets(key->name, sizeof(key->name));
        break;
    case SORT_BY_QUANTITY:
        printf("Enter quantity: ");
        scanf("%d", &key->count);
        break;
    case SORT_BY_PRICE:
        printf("Enter price: ");
        scanf("%f", &key->price);
        break;
    default:
        return 0;
    }
    return 1;
}

int (*getComparisonFunction(int type))(const void*, const void*) {
    switch (type) {
    case SORT_BY_NAME:
        return compareByName;
    case SORT_BY_QUANTITY:
        return compareByQuantity;
    case SORT_BY_PRICE:
        return compareByPrice;
    default:
        return NULL;
    }
}

int isSorted(Product** productArr, int size, int (*compareFunc)(const void*, const void*)) {
    for (int i = 0; i < size - 1; i++) {
        if (compareFunc(&productArr[i], &productArr[i + 1]) > 0) {
            return 0;
        }
    }
    return 1;
}

void saveBinInfo(SuperMarket* pMarket) {
    if (!pMarket) {
        printf("Invalid market data.\n");
        return;
    }

    FILE* filePtr = fopen(SUPERMARKET_BIN_FILE, "wb");
    if (!filePtr) {
        printf("Error: Unable to open binary file for writing.\n");
        return;
    }

    // Write market name length and name
    int marketNameLength = (int)strlen(pMarket->name) + 1;
    if (fwrite(&marketNameLength, sizeof(int), 1, filePtr) != 1 ||
        fwrite(pMarket->name, sizeof(char), marketNameLength, filePtr) != (size_t)marketNameLength) {
        printf("Error: Writing market name to file failed.\n");
        fclose(filePtr);
        return;
    }

    // Write product count
    int totalProducts = pMarket->productCount;
    if (fwrite(&totalProducts, sizeof(int), 1, filePtr) != 1) {
        printf("Error: Writing product count failed.\n");
        fclose(filePtr);
        return;
    }

    // Write each product to the file
    for (int idx = 0; idx < totalProducts; ++idx) {
        if (fwrite(pMarket->productArr[idx], sizeof(Product), 1, filePtr) != 1) {
            printf("Error: Writing product data for product #%d failed.\n", idx + 1);
            fclose(filePtr);
            return;
        }
    }

    fclose(filePtr);
    printf("SuperMarket data has been successfully saved.\n");
}

void saveTXTinfo(SuperMarket* pMarket) {
    if (!pMarket) {
        printf("Error: Invalid market data provided.\n");
        return;
    }

    FILE* filePtr = fopen(CUSTOMERS_TXT_FILE, "w");
    if (!filePtr) {
        printf("Error: Unable to open the customer file for writing.\n");
        return;
    }

    // Write the total number of customers
    if (fprintf(filePtr, "%d\n", pMarket->customerCount) < 0) {
        printf("Error: Failed to write customer count.\n");
        fclose(filePtr);
        return;
    }

    // Loop through customers and write their data
    for (int idx = 0; idx < pMarket->customerCount; ++idx) {
        Customer* currentCustomer = &pMarket->customerArr[idx];

        if (fprintf(filePtr, "%s\n%s\n", currentCustomer->name, currentCustomer->id) < 0) {
            printf("Error: Failed to write customer details.\n");
            fclose(filePtr);
            return;
        }

        if (currentCustomer->pDerivedObj != NULL) {
            ClubMember* member = (ClubMember*)currentCustomer->pDerivedObj;
            if (fprintf(filePtr, "1 %d\n", member->totalMonths) < 0) {
                printf("Error: Failed to write club member data.\n");
                fclose(filePtr);
                return;
            }
        }
        else {
            if (fprintf(filePtr, "0\n") < 0) {
                printf("Error: Failed to write non-member status.\n");
                fclose(filePtr);
                return;
            }
        }
    }

    fclose(filePtr);
    printf("Customer information has been successfully saved.\n");
}
//-------------------------------------------------------------------------------------

int loadTXTinfo(SuperMarket* pMarket) {
    if (!pMarket) {
        printf("Invalid market structure.\n");
        return 0;
    }

    FILE* filePtr = fopen(CUSTOMERS_TXT_FILE, "r");
    if (!filePtr) {
        printf("Error: Unable to open file.\n");
        return 0;
    }

    int result = loadCustomersFromFile(filePtr, pMarket);
    fclose(filePtr);

    printf(result ? "Customers loaded successfully.\n" : "Failed to load customers.\n");
    return result;
}

int loadCustomersFromFile(FILE* filePtr, SuperMarket* pMarket) {
    int totalCustomers;
    if (fscanf(filePtr, "%d\n", &totalCustomers) != 1 || totalCustomers <= 0) {
        printf("Error: Invalid customer count.\n");
        return 0;
    }

    for (int i = 0; i < totalCustomers; i++) {
        if (!loadSingleCustomer(filePtr, pMarket))
            return 0;
    }
    return 1;
}

int loadSingleCustomer(FILE* filePtr, SuperMarket* pMarket) {
    Customer tempCustomer;
    if (!readCustomerInfo(filePtr, &tempCustomer) || !appendCustomerToMarket(pMarket, &tempCustomer)) {
        printf("Error adding customer.\n");
        freeCustomer(&tempCustomer);
        return 0;
    }
    return 1;
}

int readCustomerInfo(FILE* filePtr, Customer* customer) {
    char clubStatus;
    if (!extractCustomerData(filePtr, customer, &clubStatus))
        return 0;

    if (clubStatus == '1') {
        if (!setupClubMemberFromFile(filePtr, customer))
            return 0;
    }
    else {
        customer->pDerivedObj = NULL;
        initCustomerVTable(customer);
    }
    return 1;
}

int extractCustomerData(FILE* filePtr, Customer* customer, char* clubStatus) {
    char firstName[MAX_STR_LEN] = "", lastName[MAX_STR_LEN] = "", id[MAX_STR_LEN] = "";
    if (!getCustomerName(filePtr, firstName, lastName) || !getCustomerID(filePtr, id, clubStatus))
        return 0;
    return assignCustomerDetails(customer, firstName, lastName, id);
}

int getCustomerName(FILE* filePtr, char* firstName, char* lastName) {
    char buffer[MAX_STR_LEN];
    while (fscanf(filePtr, "%s", buffer) == 1) {
        if (strcmp(buffer, "-") == 0) break;
        if (strlen(firstName) + strlen(buffer) + 2 > MAX_STR_LEN) return 0;
        strcat(firstName, strlen(firstName) > 0 ? " " : "");
        strcat(firstName, buffer);
    }
    return fscanf(filePtr, "%s", lastName) == 1;
}

int getCustomerID(FILE* filePtr, char* id, char* clubStatus) {
    return fscanf(filePtr, "%s %c", id, clubStatus) == 2;
}

int assignCustomerDetails(Customer* customer, const char* firstName, const char* lastName, const char* id) {
    char* fullName = combineFirstLast((char* []) { (char*)firstName, (char*)lastName });
    if (!fullName) return 0;

    customer->name = strdup(fullName);
    customer->id = strdup(id);
    customer->pCart = NULL;
    free(fullName);

    if (!customer->name || !customer->id) {
        freeCustomer(customer);
        return 0;
    }
    return 1;
}

int setupClubMemberFromFile(FILE* filePtr, Customer* customer) {
    customer->pDerivedObj = malloc(sizeof(ClubMember));
    if (!customer->pDerivedObj) return 0;

    if (fscanf(filePtr, "%d\n", &((ClubMember*)customer->pDerivedObj)->totalMonths) != 1) {
        free(customer->pDerivedObj);
        customer->pDerivedObj = NULL;
        return 0;
    }
    customer->vtable = NULL;
    initClubMemberVTableHelper(customer);
    return 1;
}


int loadBinInfo(SuperMarket* pMarket) {
    if (!pMarket) {
        printf("Invalid market structure.\n");
        return 0;
    }

    FILE* filePtr = fopen(SUPERMARKET_BIN_FILE, "rb");
    if (!filePtr) {
        printf("Error: Unable to open file.\n");
        return 0;
    }

    int result = processSuperMarketData(filePtr, pMarket);
    fclose(filePtr);

    printf(result ? "SuperMarket loaded successfully.\n" : "Failed to load SuperMarket.\n");
    return result;
}

int processSuperMarketData(FILE* filePtr, SuperMarket* pMarket) {
    if (!loadMarketName(filePtr, &pMarket->name) ||
        !loadProducts(filePtr, &pMarket->productArr, &pMarket->productCount)) {
        free(pMarket->name);
        return 0;
    }
    return 1;
}

int loadMarketName(FILE* filePtr, char** marketName) {
    int nameLength = 0;
    if (fread(&nameLength, sizeof(int), 1, filePtr) != 1) {
        printf("Error reading market name length.\n");
        return 0;
    }

    *marketName = (char*)malloc(nameLength);
    if (!*marketName || fread(*marketName, sizeof(char), nameLength, filePtr) != (size_t)nameLength) {
        printf("Error reading market name.\n");
        return 0;
    }
    return 1;
}

int loadProducts(FILE* filePtr, Product*** productList, int* productCount) {
    if (fread(productCount, sizeof(int), 1, filePtr) != 1 || *productCount <= 0) {
        printf("Invalid product count.\n");
        return 0;
    }

    *productList = (Product**)malloc(*productCount * sizeof(Product*));
    if (!*productList) {
        printf("Memory allocation error.\n");
        return 0;
    }

    return fetchProductDetails(filePtr, *productList, *productCount);
}

int fetchProductDetails(FILE* filePtr, Product** productList, int count) {
    for (int i = 0; i < count; i++) {
        if (!retrieveProduct(filePtr, &productList[i])) {
            printf("Error reading product %d.\n", i + 1);
            freeProductList(productList, i);
            return 0;
        }
    }
    return 1;
}

int retrieveProduct(FILE* filePtr, Product** productPtr) {
    *productPtr = (Product*)malloc(sizeof(Product));
    if (!*productPtr) {
        printf("Memory allocation for product failed.\n");
        return 0;
    }

    if (fread(*productPtr, sizeof(Product), 1, filePtr) != 1) {
        printf("Failed to read product data.\n");
        free(*productPtr);
        return 0;
    }
    return 1;
}

void freeProductList(Product** productList, int count) {
    for (int i = 0; i < count; i++) {
        free(productList[i]);
    }
    free(productList);
}



//---------------------------------end of binary-------------------------------------

void freeMarket(SuperMarket* pMarket) {
    free(pMarket->name);
    for (int i = 0; i < pMarket->productCount; i++) {
        freeProduct(pMarket->productArr[i]);
        free(pMarket->productArr[i]);
    }
    free(pMarket->productArr);

    for (int i = 0; i < pMarket->customerCount; i++)
        freeCustomer(&pMarket->customerArr[i]);
    free(pMarket->customerArr);
}



void generalArrayFunction(void* array, int elementCount, size_t elementSize, void (*func)(void*)) {
    if (!array || !func || elementCount <= 0 || elementSize <= 0) {
        printf("Invalid parameters.\n");
        return;
    }

    for (int i = 0; i < elementCount; i++) {
        void* element = (char*)array + (i * elementSize);
        func(element);
    }
}
