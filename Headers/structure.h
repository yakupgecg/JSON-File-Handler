#pragma once

#ifndef HASHMAP_STRUCT_H
#define HASHMAP_STRUCT_H

/* This header defines structs, macros and variables */

#ifndef RAW
#define RAW 0 // Raw (For example a string, or a null in JSON)
#endif

#ifndef SHR
#define SHR 1 // Short
#endif

#ifndef INT
#define INT 2 // Integer
#endif

#ifndef FLT
#define FLT 3 // Float
#endif

#ifndef DBL
#define DBL 4 // Double
#endif

#ifndef LONG
#define LONG 5 // Long
#endif

#ifndef LL
#define LL 6 // Long long
#endif

#ifndef LDBL
#define LDBL 7  // Long Double
#endif

#ifndef LIST
#define LIST 8 // Arrays
#endif

#ifndef NMAP
#define NMAP 9 // Nested objects
#endif

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
    int valuetype;
    struct Hmap *next;
} obj_t; // Object

typedef struct Linkedlist {
    void *value;
    int valuetype;
    struct Linkedlist *next;
} array_t;

#endif // HASHMAP_STRUCT_H