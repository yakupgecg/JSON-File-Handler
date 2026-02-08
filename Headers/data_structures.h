#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#ifndef HASHMAP_STRUCT_H
#define HASHMAP_STRUCT_H

/* This header defines structs, valuetypes, macros and variables */

enum jfh_valuetype {
    JFH_STR,   // String (can also be bool, null or any number)
    JFH_INT,   // Integer
    JFH_DBL,   // Double
    JFH_LIST,  // Nested list
    JFH_OBJ,    // Nested map
    JFH_BOOL,   // Boolean
    JFH_NULL   // Null (can also be strings without quotes, but not practical and illegal in json)
};

#ifndef JFH_EJSON
#define JFH_EJSON 200 // For errors with json strings
#endif

typedef struct jfh_Hmap jfh_obj_t;
typedef struct jfh_Linkedlist jfh_array_t;

typedef struct {
    enum jfh_valuetype vt;
    union {
        char *str;
        int64_t i;
        double dbl;
        bool b;
        jfh_obj_t *obj;
        jfh_array_t *arr;
    } value;
} jfh_json_value_t;

typedef struct jfh_Hmap {
    char *key;
    jfh_json_value_t value;
    struct jfh_Hmap *next;
    struct jfh_Hmap *prev;
    bool empty;
} jfh_obj_t; // Object

typedef struct jfh_Linkedlist {
    jfh_json_value_t value;
    struct jfh_Linkedlist *next;
    struct jfh_Linkedlist *prev;
    bool empty;
} jfh_array_t; // Array

#endif // HASHMAP_STRUCT_H