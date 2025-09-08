#pragma once

#ifndef JSON_H
#define JSON_H

/* This header includes all the other header files so you do not have to type and spending incredibly useful seconds of your life on useless extra writing

#include "parser.h"
#include "structure.h"
#include "json-functions.h"
#include "functions.h"

Which takes incredibly much time over including 4 headers, you can just type

#include "JSON.h"

This can save in total of (((4 * (len("#include") + 4)) + len("parser.h") + len("structure.h") + len("json-encoder.h") + len("functions.h")) - 3 - len("#include") - len("JSON.h")) bytes,
and very much time for you to do other stuff than to stupidly type four headers

Time to do some math my fellow JSON'ers
*/

#include "parser.h"
#include "structure.h"
#include "json-functions.h"
#include "functions.h"

/*

Here is a list of all the features coming soon
    -Function that could find any errors on JSON strings or files
    -JSON decoder
    -More advanced indent function that will indent based on a setting.json file (only when JSON decoder is done)
    -Finish the project by fixing bugs and ensuring memory management is safe

At the time I have made all these updates and improvements
this project will probably be in v1.0.0.

I might be adding new and better improvements but still rarely, since
after v1.0.0 I will work on a new project.

The release date (v1.0.0) of this project is still being planned but this might take time since some features are still missing.
Long live this project.

*/

#endif