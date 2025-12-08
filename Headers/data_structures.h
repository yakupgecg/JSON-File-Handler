#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#ifndef HASHMAP_STRUCT_H
#define HASHMAP_STRUCT_H

/* This header defines structs, valuetypes, macros and variables */

enum jfh_valuetype {
    JFH_STR,   // String (can also be bool, null or any number)
    JFH_INT,   // Integer
    JFH_DBL,   // Double
    JFH_LIST,  // Nested list
    JFH_OBJ   // Nested map
};

#ifndef JFH_EJSON
#define JFH_EJSON 200 // For errors with json strings
#endif

// These variables below are for encoding functions to allocate enough memory for each type
extern unsigned int JFH_SHR_STR_LEN;   // The maximum length of a string made from a Short
extern unsigned int JFH_INT_STR_LEN;   // The maximum length of a string made from an Integer
extern unsigned int JFH_FLT_STR_LEN;   // The maximum length of a string made from a Float
extern unsigned int JFH_DBL_STR_LEN;   // The maximum length of a string made from a Double
extern unsigned int JFH_LONG_STR_LEN;  // The maximum length of a string made from a Long
extern unsigned int JFH_LL_STR_LEN;    // The maximum length of a string made from a Long long
extern unsigned int JFH_LDBL_STR_LEN;  // The maximum length of a string made from a Long double
extern size_t JFH_PARSER_KEYSIZE; // The allocation size of the key in the parser
extern size_t JFH_PARSER_VALSIZE; // The allocation size of the value in the parser

typedef struct jfh_Hmap jfh_obj_t;
typedef struct jfh_Linkedlist jfh_array_t;

typedef struct {
    enum jfh_valuetype vt;
    union {
        struct {
            char *str;
            unsigned int len;
        } str;
        int i;
        double dbl;
        jfh_obj_t *obj;
        jfh_array_t *arr;
    } value;
} jfh_json_value_t;

typedef struct jfh_Hmap {
    char *key;
    jfh_json_value_t value;
    struct jfh_Hmap *next;
    struct jfh_Hmap *prev;
} jfh_obj_t; // Object

typedef struct jfh_Linkedlist {
    jfh_json_value_t value;
    struct jfh_Linkedlist *next;
    struct jfh_Linkedlist *prev;
} jfh_array_t; // Array

#endif // HASHMAP_STRUCT_H