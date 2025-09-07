#pragma once

#ifndef HASHMAP_STRUCT_H
#define HASHMAP_STRUCT_H

/* This header defines structs, macros and variables */

enum valuetype {RAW, SHR, INT, FLT, DBL, LONG, LL, LDBL, LIST, NMAP};

#ifndef EJSON
#define EJSON 200 // For errors with json strings
#endif

extern unsigned int SHR_STR_LEN;
extern unsigned int INT_STR_LEN;
extern unsigned int FLT_STR_LEN;
extern unsigned int DBL_STR_LEN;
extern unsigned int LONG_STR_LEN;
extern unsigned int LL_STR_LEN;
extern unsigned int LDBL_STR_LEN;

typedef struct Hmap {
    char *key;
    void *value;
    enum valuetype valuetype;
    struct Hmap *next;
} obj_t; // Object

typedef struct Linkedlist {
    void *value;
    enum valuetype valuetype;
    struct Linkedlist *next;
} array_t;

#endif // HASHMAP_STRUCT_H