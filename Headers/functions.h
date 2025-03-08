#pragma once

#ifndef HASHMAP_FUNCS_H
#define HASHMAP_FUNCS_H

#include "structure.h"

// Returns the length of the hashmap, but if the pair is not the root of the hashmap, it will start from pair
unsigned int maplen(map_t *);

// Frees all the pairs after map and itself
int freemap(map_t *);

// Forcefully frees a pair
void freepair(map_t *);

// Returns the hash map, which has the key to find
map_t *getpairbykey(map_t *, char *);

// This will initalize a hashmap and then return a pointer to it
map_t *initializemap();

// Adds a pair to the end of the given map
int addlast(map_t *);

// Adds another pair after the given pair
int addafter(map_t *);

// Removes the last pair in a hashmap
int removelast(map_t *);
// Resets pairs key to the given string
int resetkey(map_t *, char *);

// Resets pairs value to the given integer
int restoint(map_t *, int);

// Resets pairs value to the given short
int restoshort(map_t *, short);

// Resets pairs value to the given float
int restofloat(map_t *, float);

// Resets pairs value to the given string
int restoraw(map_t *, char *, int);

// Resets pairs value to the given double
int restodouble(map_t *, double);

// Resets pairs value to the given long
int restolong(map_t *, long);

// Resets pairs value to the given long long
int restolonglong(map_t *, long long);

// Resets pairs value to the given long double
int restolongdouble(map_t *, long double);

#endif