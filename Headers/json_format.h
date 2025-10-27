#pragma once

#ifndef JSONENCODER_H
#define JSONENCODER_H

#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

/*  Includes basically every function that are larger than normal utility
    functions and includes string handling */

static char *remove_whitespace(char *);
char *indent_json(char *, size_t);
obj_t *decode_pair(char*);

#endif