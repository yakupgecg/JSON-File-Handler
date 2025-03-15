#pragma once

#ifndef PARSER_H
#define PARSER_H

/* This header file defines functions that parses integer and floating point types to
string representations. For example:

intparse(8) returns "8"

Even though we have value types (like SHR, FLT, LONG), We strongly recommend using parsers because the
buffer sizer in the encoder calculates the needed buffer size using value types, since the buffer sizer can
never know the values string representation length, but if it's raw data (valuetype RAW), the buffer sizer can simply
just calculate the string length. */

char *shortparse(short);
char *intparse(int);
char *floatparse(float);
char *doubleparse(double);
char *longparse(long);
char *llparse(long long);
char *ldparse(long double);

#endif