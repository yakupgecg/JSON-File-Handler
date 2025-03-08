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

// Returns the pair that has the given key
map_t *getpairbykey(map_t *, char *);

// Returns the pair before the given pair that has the given key, if found
map_t *getprpairbykey(map_t *, char *);

// This will initalize a hashmap and then return a pointer to it
map_t *initializemap();

// Adds a pair to the end of the given map
int appendH(map_t *);

// Adds another pair after the given pair
int addafterH(map_t *);

// Removes the last pair in a hashmap
int removelastH(map_t *);

// Resets pairs key to the given string
int resetkey(map_t *, char *, unsigned int);

// Resets pairs value to the given integer
int restointH(map_t *, int);

// Resets pairs value to the given short
int restoshortH(map_t *, short);

// Resets pairs value to the given float
int restofloatH(map_t *, float);

// Resets pairs value to the given string
int restorawH(map_t *, char *, int);

// Resets pairs value to the given double
int restodoubleH(map_t *, double);

// Resets pairs value to the given long
int restolongH(map_t *, long);

// Resets pairs value to the given long long
int restolonglongH(map_t *, long long);

// Resets pairs value to the given long double
int restolongdoubleH(map_t *, long double);

#endif