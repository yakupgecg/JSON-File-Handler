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

static int stobj_encoder(jfh_obj_t *, char **, char **, size_t *, size_t *);
static int starr_encoder(jfh_array_t *, char **, char **, size_t *, size_t *);
char *JFH_encode_obj(jfh_obj_t *);
char *JFH_encode_arr(jfh_array_t *);
char *JFH_indent_json(char *, size_t);
jfh_obj_t *JFH_decode_pair(char*);

#endif