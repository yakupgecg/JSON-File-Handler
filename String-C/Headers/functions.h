#pragma once

#include "structure.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Declares functions for handling ctr_t

ctr_t *init_ctr(char);
ctrm_t *init_ctrm(ctr_t *, ctr_t *);
void free_ctr(ctrm_t *);
unsigned int length_ctr(ctrm_t *);
ctrm_t *to_ctr(char *);
char *ctr_to_string(ctrm_t *);
ctr_t *get_ctr_byindex(ctrm_t *, unsigned int);
ctr_t *add_ctr_e(ctrm_t *, char);
ctr_t *add_ctr_a(ctr_t *, char, ctrm_t*);
ctr_t *pop_ctr_e(ctrm_t *);
ctr_t *pop_ctr_a(ctr_t *);
ctr_t *copy_ctr(ctr_t *);
ctrm_t *copy_ctrm(ctrm_t *);