#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "main.h"
#include "General.h"
#include "Supermarket.h"

int main()
{
	SuperMarket	market;

	srand((unsigned int)time(NULL));

	if (!initSuperMarket(&market))
	{
		printf("error init Super Market");
		return 0;
	}

	int option;
	int stop = 0;

	do
	{
		option = menu();
		switch (option)
		{
		case eShowSuperMarket:
			printSuperMarket(&market);
			break;

		case eAddProduct:
			if (!addProduct(&market))
				printf("Error adding product\n");
			break;

		case eAddCustomer:
			if (!addCustomer(&market))
				printf("Error adding customer\n");
			break;

		case eCustomerDoShopping:
			if (!doShopping(&market))
				printf("Error in shopping\n");
			break;

		case ePrintCart:
			doPrintCart(&market);
			break;

		case eCustomerManageShoppingCart:
			if (!manageShoppingCart(&market))
				printf("Error in shopping cart managment\n");
			break;

		case ePrintProductByType:
			printProductByType(&market);
			break;

		case eSortArrayByType:
			sortByType(&market);
			break;

		case eSearchArrayByType:
			searchByType(&market);
			break;

		case EXIT:
			saveBinInfo(&market);
			saveTXTinfo(&market);
			printf("Bye bye\n");
			stop = 1;
			break;

		default:
			printf("Wrong option\n");
			break;
		}
	} while (!stop);


	freeMarket(&market);

	return 1;
}

int menu()
{
	int option;
	printf("\n");
	printf("Please choose one of the following options\n");
	for (int i = 0; i < eNofOptions; i++)
		printf("%d - %s\n", i, menuStrings[i]);
	printf("%d - Quit\n", EXIT);
	scanf("%d", &option);
	//clean buffer
	char tav;
	scanf("%c", &tav);
	return option;
}

