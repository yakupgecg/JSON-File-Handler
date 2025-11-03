#pragma once

#ifndef HASHMAP_FUNCS_H
#define HASHMAP_FUNCS_H

#include "data_structures.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/* This header defines functions for handling hash maps and lists */

//Returns the valuetype as string representation.
char *get_vt(enum valuetype);

// Returns the length of the hashmap, but if the pair is not the root of the hashmap, it will start from pair
unsigned int map_len(obj_t *);

// Returns the length of the list
unsigned int list_len(array_t *);

// Counts the length of the given array
int list_size(array_t *);

// Counts the length of the given hashmap
int map_size(obj_t *);

//Frees a json value
int free_json_value(json_value_t);

// Forcefully frees a pair
int free_pair(obj_t *);

// Forcefully frees an element
int free_element(array_t *);

// Frees all the pairs after map and itself
int free_map(obj_t *);

// Frees the list (or frees every element after the given element)
int free_list(array_t *);

// Returns the pair that has the given key
obj_t *pairbykey(obj_t *, char *);

// Returns the element by index in the given list
array_t *getelementbyindex(array_t *, unsigned int);

// Returns the pair before the given pair that has the given key, if found
obj_t *pr_pairbykey(obj_t *, char *);

// This will initalize a hashmap and then return a pointer to it
obj_t *initM();

// Returns the last pair in a hash map
obj_t *last_pair(obj_t *);

// Returns the last element in a list
array_t *last_element(array_t *);

// Initializes a list
array_t *initL();

// Adds a pair to the end of the given map and returns it
obj_t *appendH(obj_t *);

// Adds an element to the end of the given list and returns it
array_t *appendL(array_t *);

// Adds another pair after the given pair and returns it
obj_t *insertH(obj_t *);

// Adds another element after the given element and returns it
array_t *insertL(array_t *);

// Removes the last pair in a hashmap
int r_lastH(obj_t *);

// Removes the last element in a list
int r_lastL(array_t *);

// Removes the given map and then reassigns the next pointed the pair before the given map to pair after the given map
int r_afterH(obj_t *, obj_t *);

// Removes the element after the given element
int r_afterL(array_t *);

// Resets pairs key to the given string
obj_t *resetkey(obj_t *, char *);

// Resets pairs value to the given integer
int setintH(obj_t *, int);

int setintL(array_t *, int);

// Resets pairs value to the given short
int setshortH(obj_t *, short);

int setshortL(array_t *, short);

// Resets pairs value to the given float
int setfloatH(obj_t *, float);

int setfloatL(array_t *, float);

// Resets pairs value to the given string
int setrawH(obj_t *, char *);

int setrawL(array_t *, char *);

// Resets pairs value to the given double
int setdoubleH(obj_t *, double);

int setdoubleL(array_t *, double);

// Resets pairs value to the given long
int setlongH(obj_t *, long);

int setlongL(array_t *, long);

// Resets pairs value to the given long long
int setlonglongH(obj_t *, long long);

int setlonglongL(array_t *, long long);

// Resets pairs value to the given long double
int setlongdoubleH(obj_t *, long double);

int setlongdoubleL(array_t *, long double);

// Resets pairs value to the given list
int setlistH(obj_t *, array_t *);

int setlistL(array_t *, array_t *);

// Resets pairs value to the given hashmap
int setmapH(obj_t *, obj_t *);

int setmapL(array_t *, obj_t *);

#endif