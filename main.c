// MAIN IDEA: Make this safe enough for basic JSON file formatting

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RAW -1 // For making a string without double quotes. For example making a null in JSON file format
#define SHR 0
#define STR 1
#define INT 2
#define FLT 3
#define DBL 4
#define LONG 5
#define LL 6
#define LDBL 7

typedef struct Hmap {
    char *key;
    void *value;
    int valuetype;
    struct Hmap *next;
} map_t;

// This will initalize a hashmap and then return a pointer to it.
map_t *initalizemap() {
    map_t* map = malloc(sizeof(map_t));
    if (map == NULL) {
        return NULL;
    }
    map->next = NULL;
    return map;
}

// Next: make a function that adds a pair after a map

int resetkey(map_t *pair, char *key) {
    free(pair->key);
    pair->key = strdup(key);
    if (pair->key == NULL) {
        return 1;
    }
    return 0;
}

int restoint(map_t *pair, int value) {
    pair->value = malloc(sizeof(int));
    if (pair->value == NULL) {
        return 1;
    }
    *(int*)pair->value = value;
    pair->valuetype = INT;
    return 0;
}

int restoshort(map_t *pair, short value) {
    pair->value = malloc(sizeof(short));
    if (pair->value == NULL) {
        return 1;
    }
    *(short*)pair->value = value;
    pair->valuetype = SHR;
    return 0;
}

int restofloat(map_t *pair, float value) {
    pair->value = malloc(sizeof(float));
    if (pair->value == NULL) {
        return 1;
    }
    *(float*)pair->value = value;
    pair->valuetype = FLT;
    return 0;
}

int restostring(map_t *pair, char *val_ptr, int str_len) {
    pair->value = malloc(str_len * sizeof(char));
    if (pair->value == NULL) {
        return 1;
    }
    pair->value = strdup(val_ptr);
    pair->valuetype = STR;
    return 0;
}

int restodouble(map_t *pair, double value) {
    pair->value = malloc(sizeof(double));
    if (pair->value == NULL) {
        return 1;
    }
    *(double*)pair->value = value;
    pair->valuetype = DBL;
    return 0;
}

int restolong(map_t *pair, long value) {
    pair->value = malloc(sizeof(long));
    if (pair->value == NULL) {
        return 1;
    }
    *(long*)pair->value = value;
    pair->valuetype = LONG;
    return 0;
}

int restolonglong(map_t *pair, long long value) {
    pair->value = malloc(sizeof(long long));
    if (pair->value == NULL) {
        return 1;
    }
    *(long long*)pair->value = value;
    pair->valuetype = LL;
    return 0;
}

int restolongdouble(map_t *pair, long double value) {
    pair->value = malloc(sizeof(long double));
    if (pair->value == NULL) {
        return 1;
    }
    *(long double*)pair->value = value;
    pair->valuetype = LDBL;
    return 0;
}

// Returns the length of the hashmap, but if the pair is not the root of the hashmap, it will start from pair
size_t maplen(map_t *map) {
    size_t len = 0;
    map_t *current = map;
    while (current != NULL) {
        len += 1;
        current = current->next;
    }
    return len;
}

// Forgot to mention the update about this function in the last commit
void freemap(map_t* map) {
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
}

// Forcefully frees a pair (pair will still be displayed by mapToJson or pairToJson function)
void freepair(map_t *pair) {
    if (pair->key != NULL) {
        free(pair->key);
    }
    if (pair->value != NULL) {
        free(pair->value);
    }
    free(pair);
}

int main() {

}