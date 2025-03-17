#pragma once

#ifndef JSONENCODER_H
#define JSONENCODER_H

#include "functions.h"

/* This header defines the encoder. And if I will ever make the decoder, it will be here.
I made made functions seperately from functions.h since they serve a purpose for JSON files */

char *pairtoJSON(map_t*);
char *maptoJSON(map_t*);
char *listoJSON(list_t*);

#endif