#pragma once

#ifndef JSONENCODER_H
#define JSONENCODER_H

#include "utils.h"

/* This header defines the encoder. And if I will ever make the decoder, it will be here.
I made made functions seperately from functions.h since they serve a purpose for JSON files */

char *indent_json(char *, unsigned int, unsigned int);
obj_t *decode_pair(char*);

#endif