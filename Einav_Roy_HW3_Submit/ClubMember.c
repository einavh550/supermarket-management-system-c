#include <stdlib.h>
#include <stdio.h>
#include "ClubMember.h"
#include "Customer.h"



int initClubMember(Customer* pCustomer) {
    if (!initCustomer(pCustomer))
        return 0;

    pCustomer->pDerivedObj = (ClubMember*)malloc(sizeof(ClubMember));
    if (!pCustomer->pDerivedObj)
    {
        freeCustomer(pCustomer);
        return 0;
    }

    ClubMember* CustomerMember = (ClubMember*)pCustomer->pDerivedObj;
    printf("Please Enter total months left to your membership: ");
    scanf("%d", &CustomerMember->totalMonths);

    initClubMemberVTableHelper(pCustomer);

    return 1;
}

void initClubMemberVTableHelper(Customer* pCustomer) {
    if (!pCustomer->vtable) {
        pCustomer->vtable = malloc(sizeof(CustomerVTable));
        if (!pCustomer->vtable) {
            printf(" vtable allocation not operated.\n");
            return;
        }
    }
    pCustomer->vtable->init = initClubMember;
    pCustomer->vtable->print = printClubMember;
    pCustomer->vtable->calculateDiscount = determineClubMemberDiscount;
}

    



void printClubMember(const Customer* pCustomer) {
    printCustomer(pCustomer);
    const ClubMember* CustomerMember = (const ClubMember*)pCustomer->pDerivedObj;

    for (int i = 0; i < 1; i++) {
        if (CustomerMember->totalMonths > 0) {
            printf("Club Membership valid for %d months.\n", CustomerMember->totalMonths);
        }
    }
}

float determineClubMemberDiscount(const Customer* pCustomer, float totalPrice) {
    const ClubMember* CustomerMember = (const ClubMember*)pCustomer->pDerivedObj;

    float discount = 1.0;
    if (CustomerMember->totalMonths <= 24) {
        discount -= 0.001f * CustomerMember->totalMonths;
    }
    else if (CustomerMember->totalMonths <= 60) {
        discount -= (0.025f + 0.005f * ((CustomerMember->totalMonths - 24) / 12));
    }
    else {
        discount = 0.925f;  // Apply 7.5% discount
    }

    return totalPrice * discount;
}

void freeClubMemberData(Customer* pCustomer) {
    if (pCustomer->pDerivedObj != NULL) {
        free(pCustomer->pDerivedObj);
    }
    freeCustomer(pCustomer);
}
