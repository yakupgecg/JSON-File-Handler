#pragma once

#ifndef PARSER_H
#define PARSER_H

// Don't confuse this file with decoding or parsing json strings

/* This header file defines functions that parses integer and floating point types to
string representations. For example:

parseInt(8) returns "8" (strings)
Even though you can use set (like setint or setdouble) functions other than setraw, parsing approach is mostly better, 
since the encoder calculates the string's length manually.
So instead of typing like

setintH(obj, 8);

you can type

setrawH(obj, parseInt(8));
*/

char *parseShort(short);
char *parseInt(int);
char *parseFloat(float);
char *parseDouble(double);
char *parseLong(long);
char *parseLl(long long);
char *parseLd(long double);

#endif