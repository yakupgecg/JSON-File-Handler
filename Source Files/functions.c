#include "..\Headers\functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Returns the length of the hashmap, but if the pair is not the root of the hashmap, it will start from pair
unsigned int maplen(map_t *map) {
    unsigned int len = 0;
    map_t *current = map;
    while (current != NULL) {
        len += 1;
        current = current->next;
    }
    return len;
}

// Frees all the pairs after map and itself
int freemap(map_t* map) {
    if (map == NULL) {
        return 1;
    }
    map_t* current = map;
    map_t* next;
    while (current != NULL) {
        next = current->next;
        if (current->key != NULL) {
            free(current->key);
        }
        if (current->value != NULL) {
            free(current->value);
        }
        free(current);
        current = next;
    }
    return 0;
}

// Forcefully frees a pair
void freepair(map_t *pair) {
    if (pair->key != NULL) {
        free(pair->key);
    }
    if (pair->value != NULL) {
        free(pair->value);
    }
    free(pair);
}

// Returns the hash map, which has the key to find
map_t *getpairbykey(map_t *root, char *key) {
    if (root == NULL) {
        return NULL;
    }
    map_t *current = root;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Returns the previous pair before the pair that has the key to find, if found
map_t *getprpairbykey(map_t *root, char *key) {
    if (root == NULL) {
        return NULL;
    }
    map_t *prev = root;
    if (strcmp(prev->key, key) == 0) {
        return prev;
    }
    map_t *current = root->next;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return prev;
        }
        prev = current;
        current = current->next;
    }
    return NULL;
}

// This will initalize a hashmap and then return a pointer to it
map_t *initializemap() {
    map_t* map = malloc(sizeof(map_t));
    if (map == NULL) {
        return NULL;
    }
    map->key = malloc(sizeof(char));
    map->value = malloc(sizeof(int));
    map->next = NULL;
    return map;
}

// Adds a pair to the end of the given map
int appendH(map_t *map) {
    if (map == NULL) {
        return 1;
    }
    map_t *current = map;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = initializemap();
    if (current->next == NULL) {
        return 1;
    }
    return 0;
}

// Adds another pair after the given pair
int addafterH(map_t *pair) {
    if (pair == NULL) {
        return 1;
    }
    map_t *new = initializemap();
    if (pair->next != NULL) {
        new->next = pair->next;
    }
    pair->next = new;
    return 0;
}

// Removes the last pair in a hashmap
int removelastH(map_t *map) {
    if (map == NULL) {
        return 1;
    }
    if (map->next != NULL) {
        map->next = NULL;
        return 0;
    }
    map_t *current = map;
    while (current->next != NULL) {
        current = current->next;
    }
    freepair(current);
    current = NULL;
    return 0;
}

// Removes the given map and then reassigns the next pointed the pair before the given map to pair after the given map
int removeafter(map_t *root, map_t *pairtormv) {
    if (root == NULL) {
        return 1;
    }
    map_t *current = root;
    while (current != NULL) {
        if (strcmp(current->key, pairtormv->key) == 0) {
            getprpairbykey(root, pairtormv->key)->next = current->next;
            freepair(current);
            return 0;
        }
        current = current->next;
    }
    return 0;
}

// Resets pairs key to the given string
int resetkey(map_t *pair, char *key, unsigned int str_len) {
    if (pair == NULL) {
        return 1;
    }
    free(pair->key);
    pair->key = malloc(str_len + 1);
    if (pair->key == NULL) {
        return 1;
    }
    strncpy(pair->key, key, str_len);
    pair->key[str_len] = '\0';
    return 0;
}

// Resets pairs value to the given integer
int restointH(map_t *pair, int value) {
    if (pair == NULL) {
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(int));
    if (pair->value == NULL) {
        return 1;
    }
    *(int*)pair->value = value;
    pair->valuetype = INT;
    return 0;
}

// Resets pairs value to the given short
int restoshortH(map_t *pair, short value) {
    if (pair == NULL) {
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(short));
    if (pair->value == NULL) {
        return 1;
    }
    *(short*)pair->value = value;
    pair->valuetype = SHR;
    return 0;
}

// Resets pairs value to the given float
int restofloatH(map_t *pair, float value) {
    if (pair == NULL) {
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(float));
    if (pair->value == NULL) {
        return 1;
    }
    *(float*)pair->value = value;
    pair->valuetype = FLT;
    return 0;
}

// Resets pairs value to the given string
int restorawH(map_t *pair, char *val_ptr, int str_len) {
    if (pair == NULL) {
        return 1;
    }
    free(pair->value);
    pair->value = malloc(str_len + 1);
    if (pair->value == NULL) {
        return 1;
    }
    strncpy(pair->value, val_ptr, str_len);
    ((char*)pair->value)[str_len] = '\0';
    pair->valuetype = RAW;
    return 0;
}

// Resets pairs value to the given double
int restodoubleH(map_t *pair, double value) {
    if (pair == NULL) {
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(double));
    if (pair->value == NULL) {
        return 1;
    }
    *(double*)pair->value = value;
    pair->valuetype = DBL;
    return 0;
}

// Resets pairs value to the given long
int restolongH(map_t *pair, long value) {
    if (pair == NULL) {
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(long));
    if (pair->value == NULL) {
        return 1;
    }
    *(long*)pair->value = value;
    pair->valuetype = LONG;
    return 0;
}

// Resets pairs value to the given long long
int restolonglongH(map_t *pair, long long value) {
    if (pair == NULL) {
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(long long));
    if (pair->value == NULL) {
        return 1;
    }
    *(long long*)pair->value = value;
    pair->valuetype = LL;
    return 0;
}

// Resets pairs value to the given long double
int restolongdoubleH(map_t *pair, long double value) {
    if (pair == NULL) {
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(long double));
    if (pair->value == NULL) {
        return 1;
    }
    *(long double*)pair->value = value;
    pair->valuetype = LDBL;
    return 0;
}