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

// Sets the value of the object to void * (be cautious with void *)
void setvalH(obj_t*, void *, enum valuetype);
void setvalL(array_t *, void*, enum valuetype);

#endif