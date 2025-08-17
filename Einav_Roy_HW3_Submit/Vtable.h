#pragma once


struct Customer_;

typedef struct {
    int (*init)(struct Customer_*);
    void (*print)(const struct Customer_*);  // Print function
    float (*calculateDiscount)(const struct Customer_*, float);  // Discount calculation
} CustomerVTable;

