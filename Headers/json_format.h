#pragma once

#ifndef JSONENCODER_H
#define JSONENCODER_H

#include "utils.h"
#include "numtostr.h"

/*  Includes basically every function that are larger than normal utility
    functions and includes string handling
*/

static int stobj_encoder(jfh_obj_t *, char **, char **, size_t *, size_t *);
static int starr_encoder(jfh_array_t *, char **, char **, size_t *, size_t *);
char *JFH_encode_obj(jfh_obj_t *);
char *JFH_encode_arr(jfh_array_t *);
char *JFH_indent_json(char *, size_t);
static int stobj_parser(char *, jfh_obj_t **);
static int starr_parser(char *, jfh_array_t **);
jfh_obj_t *JFH_parse_obj(char *);
jfh_array_t *JFH_parse_arr(char *);

#endif