#pragma once

#ifndef HASHMAP_FUNCS_H
#define HASHMAP_FUNCS_H

#include "data_structures.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/* This header defines functions for handling objects and lists */

//Returns the valuetype as string representation.
char *get_vt(enum valuetype);

// Returns the length of the hashmap, but if the object is not the root of the hashmap, it will start from object
size_t map_len(obj_t *);

// Returns the length of the list
size_t list_len(array_t *);

//Frees a json value
int free_json_value(json_value_t *);

// Forcefully frees a object
int free_pair(obj_t *);

// Forcefully frees an element
int free_element(array_t *);

// Frees all the pairs after map and itself
int free_map(obj_t *);

// Frees the list (or frees every element after the given element)
int free_list(array_t *);

// Returns the object that has the given key
obj_t *pairbykey(obj_t *, char *);

// Returns the element by index in the given list
array_t *getelementbyindex(array_t *, size_t);

// This will initalize a hashmap and then return a pointer to it
obj_t *initM();

// Returns the last object in a hash map
obj_t *last_pair(obj_t *);

// Returns the last element in a list
array_t *last_element(array_t *);

// Initializes a list
array_t *initL();

// Adds a object to the end of the given map and returns it
obj_t *appendH(obj_t *);

// Adds an element to the end of the given list and returns it
array_t *appendL(array_t *);

//Adds an object after the given obj
obj_t *insertH(obj_t *);

//Adds an element after the given element
array_t *insertL(array_t *);

// Removes the last object and returns it
obj_t *popH(obj_t *);

// Removes the last element and returns it
array_t *popL(array_t *);

// Resets pairs key to the given string
obj_t *resetkey(obj_t *, char *);

// Sets the value of the json_value_t to void * depending on the valuetype (be cautious with void * and valuetype)
void setval(json_value_t*, void *, enum valuetype);

// Sets the object's or element's value to a string
obj_t *setstrH(obj_t *, char *, char *);
array_t *setstrL(array_t *, char *);

// Sets the object's or element's value to an integer
obj_t *setintH(obj_t *, char *, int);
array_t *setintL(array_t *, int);

// Sets the object's or element's to a double
obj_t *setdoubleH(obj_t *, char *, double);
array_t *setdoubleL(array_t *, double);

// Sets the object's or element's to an object
obj_t *setobjH(obj_t *, char *, obj_t *);
array_t *setobjL(array_t *, obj_t *);

// Sets the object's or element's to an array
obj_t *setarrH(obj_t *, char *, array_t *);
array_t *setarrL(array_t *, array_t *);

// Copies the given object, either returns the copy or copies to another object.
obj_t *copy_obj(obj_t *, obj_t *);

// Copies the given element, either returns the copy or copies to another element.
array_t *copy_element(array_t *, array_t *);

// Copies the given map, either returns the copy or copies the whole map to another map.
obj_t *copy_map(obj_t *);

// Copies the given list, either returns the copy or copies the whole list to another list.
array_t *copy_list(array_t *);

#endif