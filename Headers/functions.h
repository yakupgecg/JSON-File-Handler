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

// Adds a pair to the end of the given map and returns it
map_t *appendH(map_t *);

// Adds another pair after the given pair and returns it
map_t *addafterH(map_t *);

// Removes the last pair in a hashmap
int removelastH(map_t *);

// Removes the given map and then reassigns the next pointed the pair before the given map to pair after the given map
int removeafterH(map_t *, map_t *);

// Resets pairs key to the given string
int resetkey(map_t *, char *, unsigned int);

// Resets pairs value to the given integer
int setintH(map_t *, int);

// Resets pairs value to the given short
int setshortH(map_t *, short);

// Resets pairs value to the given float
int setfloatH(map_t *, float);

// Resets pairs value to the given string
int setrawH(map_t *, char *, int);

// Resets pairs value to the given double
int setdoubleH(map_t *, double);

// Resets pairs value to the given long
int setlongH(map_t *, long);

// Resets pairs value to the given long long
int setlonglongH(map_t *, long long);

// Resets pairs value to the given long double
int setlongdoubleH(map_t *, long double);

#endif