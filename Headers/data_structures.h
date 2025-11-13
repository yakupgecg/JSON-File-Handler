#pragma once

#ifndef HASHMAP_STRUCT_H
#define HASHMAP_STRUCT_H

/* This header defines structs, valuetypes, macros and variables */

enum valuetype {
    STR,   // String (can also be bool, null or any number)
    INT,   // Integer
    DBL,   // Double
    LIST,  // Nested list
    OBJ   // Nested map
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

typedef struct Hmap obj_t;
typedef struct Linkedlist array_t;

typedef struct {
    enum valuetype vt;
    union {
        struct {
            char *str;
            unsigned int len;
        } str;
        int i;
        double dbl;
        obj_t *obj;
        array_t *arr;
    } value;
} json_value_t;

typedef struct Hmap {
    char *key;
    json_value_t value;
    struct Hmap *next;
    struct Hmap *prev;
} obj_t; // Object

typedef struct Linkedlist {
    json_value_t value;
    struct Linkedlist *next;
    struct Linkedlist *prev;
} array_t; // Array

#endif // HASHMAP_STRUCT_H