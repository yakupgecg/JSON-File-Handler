#include "..\Headers\functions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Defines functions given by the header file functions.h


// Initializes a ctr_t
ctr_t *init_ctr(char c) {
    ctr_t *newstr = malloc(sizeof(ctr_t));
    if (newstr == NULL) return NULL;
    newstr->c = c;
    newstr->n = NULL;
    return newstr;
}

// Initializes a ctrm_t
ctrm_t *init_ctrm(ctr_t *head, ctr_t *tail) {
    if (head == NULL) return NULL;
    ctrm_t *newm = malloc(sizeof(ctrm_t));
    if (newm == NULL) return NULL;
    newm->head = head;
    if (tail != NULL) {
        newm->tail = tail;
    } else {
        newm->tail = NULL;
    }
    return newm;
}

// Frees the given ctr_t and all the ctr_t after the given ctrm's head ctr_t, and also frees the ctrm_t
void free_ctrm(ctrm_t *ctrm) {
    if (ctrm == NULL) return;
    ctr_t *current = ctrm->head;
    while (current != NULL) {
        ctr_t *temp = current;
        current = current->n;
        free(temp);
    }
    free(ctrm);
}

// Counts every ctr_t after the given ctrm's head ctr_t, and itself of course
unsigned int length_ctr(ctrm_t *ctrm) {
    if (ctrm == NULL) return 0;
    unsigned int len = 0;
    ctr_t *current = ctrm->head;
    while (current != NULL) {
        len++;
        current = current->n;
    }
    return len;
}

// Converts a normal string to a ctr_t type string
ctrm_t *to_ctr(char *str) {
    if (str == NULL) return NULL;
    ctr_t *current = init_ctr('\0');
    if (current == NULL) return NULL;
    ctrm_t *unmd1 = init_ctrm(current, NULL);
    if (unmd1 == NULL) return NULL;
    unsigned int i = 0;
    while (str[i] != '\0') {
        current->c = str[i];
        if (str[i + 1] != '\0') {
            current->n = init_ctr('\0');
            current = current->n;
        } else {
            current->n = init_ctr('\0');
			current->n->c = '\0';
			current->n->n = NULL;
            unmd1->tail = current;
        }
        i++;
    }
    return unmd1;
}

// Converts a ctr_t type string to a normal string
char *ctr_to_string(ctrm_t *ctrm) {
    if (ctrm == NULL) return NULL;
    ctr_t *current = ctrm->head;
    unsigned int length = length_ctr(ctrm);
    char *newstr = malloc(length+1);
    unsigned int i = 0;
    while (current != ctrm->tail) {
        newstr[i] = current->c;
        current = current->n;
        i++;
    }
    newstr[i] = current->c;
    current = current->n;
    i++;
    newstr[i] = '\0';
    return newstr;
}

// Counts up to a specific ctr_t by the given index and returns it
ctr_t *get_ctr_byindex(ctrm_t *ctrm, unsigned int index) {
    if (ctrm == NULL) return NULL;
    ctr_t *current = ctrm->head;
    if (index > length_ctr(ctrm)) return NULL;
    unsigned int i;
    for (i = 0; i < index; i++) {
        current = current->n;
    }
    return current;
}

// Adds a ctr_t after the given ctrm's tail ctr_t
ctr_t *add_ctr_e(ctrm_t *ctrm, char c) {
    if (ctrm->tail == NULL) return NULL;
    ctr_t *newctr = init_ctr(c);
    if (newctr == NULL) return NULL;
    ctrm->tail->n = newctr;
    ctrm->tail = ctrm->tail->n;
    ctrm->tail->n = init_ctr('\0');
    return newctr;
}

// Adds a ctr_t after the given ctr_t and returns it
ctr_t *add_ctr_a(ctr_t *ctr, char c, ctrm_t *ctrm) {
    if (ctr == NULL) return NULL;
    ctr_t *newctr = init_ctr('\0');
    if (newctr == NULL) return NULL;
    if (ctr->n == NULL) {
        newctr->n = NULL;
        ctr->n = newctr;
        if (ctrm != NULL) {
            ctrm->tail = newctr;
        }
    } else {
        newctr->n = ctr->n;
        ctr->n = newctr;
    }
    newctr->c = c;
    return newctr;
}

// Removes the last ctr_t and returns it (Update coming soon...)
ctr_t *pop_ctr_e(ctrm_t *ctrm) {
	ctr_t *temp = init_ctr('\0');
	if (temp == NULL) return NULL;
	temp->c = ctrm->tail->c;
	temp->n = ctrm->tail->n;
    free(ctrm->tail);
	ctrm->tail = get_ctr_byindex(ctrm, length_ctr(ctrm) - 1);
	return temp;
}

// Removes the ctr_t after the given ctr_t
ctr_t *pop_ctr_a(ctr_t *ctr) {
    ctr_t *temp = ctr->n;
    ctr->n = ctr->n->n;
    return temp;
}

// Returns a copy of the given ctr_t
ctr_t *copy_ctr(ctr_t *ctr) {
	ctr_t *newctr = init_ctr(ctr->c);
	return newctr;
}

// Returns a copy of the given ctrm_t
ctrm_t *copy_ctrm(ctrm_t *ctrm) {
	ctr_t *current = ctrm->head;
	ctr_t *current2 = copy_ctr(ctrm->head);
	ctrm_t *newctrm = init_ctrm(current2, NULL);
	while (current != NULL) {
		current2->n = copy_ctr(current->n);
		current2 = current2->n;
		current = current->n;
	}
	current2->n = NULL;
	return newctrm;
}