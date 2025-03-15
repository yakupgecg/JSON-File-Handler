#pragma once

#ifndef JSON_H
#define JSON_H

/* This header includes all the other header files so you do not have to type

#include "structure.h"
#include "json-handler.h"
#include "functions.h"

Instead you can just type

#include "JSON.h"
*/

#include "structure.h"
#include "json-handler.h"
#include "functions.h"

/*

Hello. Because there has not been many updates lately, here is a list of all the features coming soon
    -Lists like {"Example": ["foo", "bar", 12, null]}
    -A function turn numbers and floating point numbers to strings to avoid extra valuetypes
    -Nested lists like {"Example": ["foo", "bar", [5, false]]}
    -Nested objects like  {"Example": {"Foo": 12, "Bar": true}}
    -Syntax checker on JSON strings or files
    -JSON string or file decoder 
    -Finish the project by fixing bugs and ensuring memory management is safe

At the time I have made all these updates and improvements
this project will probably be in v1.0.0

I might be adding new and better improvements but still rarely, since
after v1.0.0 I will work on a new project

*/

#endif