#pragma once

#ifndef HASHMAP_STRUCT_H
#define HASHMAP_STRUCT_H

/* This header defines structs, valuetypes, macros and variables */

enum valuetype {
    RAW,   // Raw data (string)
    SHR,   // Short
    INT,   // Integer
    FLT,   // Float
    DBL,   // Double
    LONG,  // Long
    LL,    // Long Long
    LDBL,  // Long double
    LIST,  // Nested list
    NMAP   // Nested map
};

#ifndef EJSON
#define EJSON 200 // For errors with json strings
#endif

// These variables below are for encoding functions to allocate enough memory for each type
extern unsigned int SHR_STR_LEN;   // The maximum length of a string made from a Short
extern unsigned int INT_STR_LEN;   // The maximum length of a string made from an Integer
extern unsigned int FLT_STR_LEN;   // The maximum length of a string made from a Float
extern unsigned int DBL_STR_LEN;   // The maximum length of a string made from a Double
extern unsigned int LONG_STR_LEN;  // The maximum length of a string made from a Long
extern unsigned int LL_STR_LEN;    // The maximum length of a string made from a Long long
extern unsigned int LDBL_STR_LEN;  // The maximum length of a string made from a Long double

typedef struct Hmap {
    char *key;
    json_value_t value;
    struct Hmap *next;
} obj_t; // Object

typedef struct Linkedlist {
    json_value_t value;
    struct Linkedlist *next;
} array_t; // Array

typedef struct {
    enum valuetype vt;
    union {
        struct {
            char *str;
            size_t len;
        } str;
        short s;
        int i;
        float f;
        double dbl;
        long lg;
        long long lgl;
        long double ldbl;
        obj_t *obj;
        array_t *arr;
    } value;
} json_value_t;

#endif // HASHMAP_STRUCT_H