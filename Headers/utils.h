#pragma once

#ifndef HASHMAP_FUNCS_H
#define HASHMAP_FUNCS_H

#include "data_structures.h"

/* This header defines functions for handling objects and lists */

//Returns the valuetype as string representation.
char *JFH_get_vt(enum jfh_valuetype);

// Returns the length of the hashmap, but if the object is not the root of the hashmap, it will start from object
size_t JFH_map_len(jfh_obj_t *);

// Returns the length of the list
size_t JFH_list_len(jfh_array_t *);

//Frees a json value
int JFH_free_json_value(jfh_json_value_t *);

// Forcefully frees a object
int JFH_free_pair(jfh_obj_t *);

// Forcefully frees an element
int JFH_free_element(jfh_array_t *);

// Frees all the pairs after map and itself
int JFH_free_map(jfh_obj_t *);

// Frees the JFH_list (or frees every element after the given element)
int JFH_free_list(jfh_array_t *);

// Returns the object that has the given key
jfh_obj_t *JFH_pairbykey(jfh_obj_t *, char *);

// Returns the element by index in the given list
jfh_array_t *JFH_getelementbyindex(jfh_array_t *, size_t);

// This will initalize a hashmap and then return a pointer to it
jfh_obj_t *JFH_initM();

// Returns the last object in a hash map
jfh_obj_t *JFH_last_pair(jfh_obj_t *);

// Returns the last element in a list
jfh_array_t *JFH_last_element(jfh_array_t *);

// Initializes a list
jfh_array_t *JFH_initL();

// Adds a object to the end of the given map and returns it
jfh_obj_t *JFH_appendH(jfh_obj_t *);

// Adds an element to the end of the given list and returns it
jfh_array_t *JFH_appendL(jfh_array_t *);

//Adds an object after the given obj
jfh_obj_t *JFH_insertH(jfh_obj_t *);

//Adds an element after the given element
jfh_array_t *JFH_insertL(jfh_array_t *);

// Removes the last object and returns it
jfh_obj_t *JFH_popH(jfh_obj_t *);

// Removes the last element and returns it
jfh_array_t *JFH_popL(jfh_array_t *);

// Resets pairs key to the given string
jfh_obj_t *JFH_resetkey(jfh_obj_t *, char *);

// Sets the value of the json_value_t to void * depending on the valuetype (be cautious with void * and valuetype)
void JFH_setval(jfh_json_value_t*, void *, enum jfh_valuetype);

// Sets the object's or element's value to a string
jfh_obj_t *JFH_setstrH(jfh_obj_t *, char *, char *);
jfh_array_t *JFH_setstrL(jfh_array_t *, char *);

// Sets the object's or element's value to an integer
jfh_obj_t *JFH_setintH(jfh_obj_t *, char *, int);
jfh_array_t *JFH_setintL(jfh_array_t *, int);

// Sets the object's or element's to a double
jfh_obj_t *JFH_setdoubleH(jfh_obj_t *, char *, double);
jfh_array_t *JFH_setdoubleL(jfh_array_t *, double);

// Sets the object's or element's to an object
jfh_obj_t *JFH_setobjH(jfh_obj_t *, char *, jfh_obj_t *);
jfh_array_t *JFH_setobjL(jfh_array_t *, jfh_obj_t *);

// Sets the object's or element's to an array
jfh_obj_t *JFH_setarrH(jfh_obj_t *, char *, jfh_array_t *);
jfh_array_t *JFH_setarrL(jfh_array_t *, jfh_array_t *);

// Copies the given object, either returns the copy or copies to another object.
jfh_obj_t *JFH_copy_obj(jfh_obj_t *, jfh_obj_t *);

// Copies the given element, either returns the copy or copies to another element.
jfh_array_t *JFH_copy_element(jfh_array_t *, jfh_array_t *);

// Copies the given map, either returns the copy or copies the whole map to another map.
jfh_obj_t *JFH_copy_map(jfh_obj_t *, jfh_obj_t *);

// Copies the given list, either returns the copy or copies the whole list to another list.
jfh_array_t *JFH_copy_list(jfh_array_t *, jfh_array_t *);

// Copies the given json value, either returns the copy or copies the json value to 2nd parameter json value
jfh_json_value_t *JFH_copy_json_value(jfh_json_value_t *, jfh_json_value_t *);

#endif