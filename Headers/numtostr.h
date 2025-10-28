#pragma once

#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// Don't confuse this file with decoding or parsing json strings

/* This header file defines functions that turns integer and floating point types to
string representations. For example:

str_Int(8) returns "8" (strings)
Even though you can use set (like setint or setdouble) functions other than setraw, parsing approach is mostly better, 
since the encoder calculates the string's length manually.
So instead of typing like

setintH(obj, 8);

you can type

setrawH(obj, str_Int("8"));
*/

char *str_Short(short);
char *str_Int(int);
char *str_Float(float);
char *str_Double(double);
char *str_Long(long);
char *str_Ll(long long);
char *str_Ld(long double);

#endif