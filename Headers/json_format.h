#pragma once

#ifndef JSONENCODER_H
#define JSONENCODER_H

#include "utils.h"
#include "numtostr.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

/*  Includes basically every function that are larger than normal utility
    functions and includes string handling
*/

static int stobj_encoder(obj_t *, char **, char **, size_t *, size_t *);
static int starr_encoder(array_t *, char **, char **, size_t *, size_t *);
char *encode_obj(obj_t *);
char *encode_arr(array_t *);
char *indent_json(char *, size_t);
obj_t *decode_pair(char*);

#endif