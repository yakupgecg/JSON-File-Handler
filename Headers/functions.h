#pragma once

#ifndef HASHMAP_FUNCS_H
#define HASHMAP_FUNCS_H

#include "structure.h"

/* This header defines functions for handling hash maps and lists */

// Returns the length of the hashmap, but if the pair is not the root of the hashmap, it will start from pair
unsigned int maplen(map_t *);

// Returns the length of the list
unsigned int listlen(list_t *);

int calclistsize(list_t *);

int calcmapsize(map_t *);

// Forcefully frees a pair
int freepair(map_t *);

// Forcefully frees an element
int free_element(list_t *);

// Frees all the pairs after map and itself
int freemap(map_t *);

// Frees the list (or frees every element after the given element)
int freelist(list_t *);

// Returns the pair that has the given key
map_t *getpairbykey(map_t *, char *);

// Returns the element by index in the given list
list_t *getelementbyindex(list_t *, unsigned int);

// Returns the pair before the given pair that has the given key, if found
map_t *getprpairbykey(map_t *, char *);

// This will initalize a hashmap and then return a pointer to it
map_t *initializemap();

// Returns the last pair in a hash map
map_t *getlastpair(map_t *);

// Returns the last element in a list
list_t *getlastelement(list_t *);

// Initializes a list
list_t *initializelist();

// Adds a pair to the end of the given map and returns it
map_t *appendH(map_t *);

// Adds an element to the end of the given list and returns it
list_t *appendL(list_t *);

// Adds another pair after the given pair and returns it
map_t *addafterH(map_t *);

// Adds another element after the given element and returns it
list_t *addafterL(list_t *);

// Removes the last pair in a hashmap
int removelastH(map_t *);

// Removes the last element in a list
int removelastL(list_t *);

// Removes the given map and then reassigns the next pointed the pair before the given map to pair after the given map
int removeafterH(map_t *, map_t *);

// Removes the element after the given element
int removeafterL(list_t *);

// Resets pairs key to the given string
int resetkey(map_t *, char *, unsigned int);

// Resets pairs value to the given integer
int setintH(map_t *, int);

int setintL(list_t *, int);

// Resets pairs value to the given short
int setshortH(map_t *, short);

int setshortL(list_t *, short);

// Resets pairs value to the given float
int setfloatH(map_t *, float);

int setfloatL(list_t *, float);

// Resets pairs value to the given string
int setrawH(map_t *, char *, int);

int setrawL(list_t *, char *, unsigned int);

// Resets pairs value to the given double
int setdoubleH(map_t *, double);

int setdoubleL(list_t *, double);

// Resets pairs value to the given long
int setlongH(map_t *, long);

int setlongL(list_t *, long);

// Resets pairs value to the given long long
int setlonglongH(map_t *, long long);

int setlonglongL(list_t *, long long);

// Resets pairs value to the given long double
int setlongdoubleH(map_t *, long double);

int setlongdoubleL(list_t *, long double);

// Resets pairs value to the given list
int setlistH(map_t *, list_t *);

int setlistL(list_t *, list_t *);

// Resets pairs value to the given hashmap
int setmapH(map_t *, map_t *);

int setmapL(list_t *, map_t *);

#endif