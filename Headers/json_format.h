#pragma once

#ifndef JSONENCODER_H
#define JSONENCODER_H

#include "utils.h"

/*  Includes basically every function that are larger than normal utility
    functions and includes string handling */

char *indent_json(char *, unsigned int, unsigned int);
obj_t *decode_pair(char*);

#endif