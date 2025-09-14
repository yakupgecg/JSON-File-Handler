#pragma once

#ifndef JSONENCODER_H
#define JSONENCODER_H

#include "utils.h"

/* This header defines the encoder. And if I will ever make the decoder, it will be here.
I made made functions seperately from functions.h since they serve a purpose for JSON files */

char *encode_pair(obj_t*);
char *encode_map(obj_t*);
char *encode_list(array_t*);
char *indent_json(char *, unsigned int, unsigned int);

#endif