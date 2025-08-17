#pragma once

#include "Customer.h"
struct Customer_;

typedef struct {
    int totalMonths;
} ClubMember;

int initClubMember(Customer* pCustomer);
void initClubMemberVTableHelper(Customer* pCustomer);
void printClubMember(const Customer* pCustomer);
float determineClubMemberDiscount(const Customer* pCustomer, float totalPrice);
void freeClubMemberData(Customer* pCustomer);
