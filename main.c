// MAIN IDEA: Make this safe enough for basic JSON file formatting

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RAW -1 // For making a string without double quotes. For example making a null in JSON file format
#define SHR 0 // Short
#define STR 1 // String
#define INT 2 // Integer
#define FLT 3 // Float
#define DBL 4 // Double
#define LONG 5 // Long
#define LL 6 // Long long
#define LDBL 7  // Long Double

typedef struct Hmap {
    char *key;
    void *value;
    int valuetype;
    struct Hmap *next;
} map_t;

// This will initalize a hashmap and then return a pointer to it
map_t *initializemap() {
    map_t* map = malloc(sizeof(map_t));
    if (map == NULL) {
        return NULL;
    }
    map->next = NULL;
    return map;
}

// Adds a pair to the end of the given map
int addlast(map_t *map) {
    if (map == NULL) {
        return 1;
    }
    map_t *current = map->next;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = initializemap();
    if (current->next == NULL) {
        return 1;
    }
    return 0;
}


// Resets pairs key to the given string
int resetkey(map_t *pair, char *key) {
    if (pair == NULL) {
        return 1;
    }
    free(pair->key);
    pair->key = strdup(key);
    if (pair->key == NULL) {
        return 1;
    }
    return 0;
}

// Resets pairs value to the given integer
int restoint(map_t *pair, int value) {
    if (pair == NULL) {
        return 1;
    }
    pair->value = malloc(sizeof(int));
    if (pair->value == NULL) {
        return 1;
    }
    *(int*)pair->value = value;
    pair->valuetype = INT;
    return 0;
}

// Resets pairs value to the given short
int restoshort(map_t *pair, short value) {
    if (pair == NULL) {
        return 1;
    }
    pair->value = malloc(sizeof(short));
    if (pair->value == NULL) {
        return 1;
    }
    *(short*)pair->value = value;
    pair->valuetype = SHR;
    return 0;
}

// Resets pairs value to the given float
int restofloat(map_t *pair, float value) {
    if (pair == NULL) {
        return 1;
    }
    pair->value = malloc(sizeof(float));
    if (pair->value == NULL) {
        return 1;
    }
    *(float*)pair->value = value;
    pair->valuetype = FLT;
    return 0;
}

// Resets pairs value to the given string and adds double quotes to the start and end
int restostring(map_t *pair, char *val_ptr, int str_len) {
    if (pair == NULL) {
        return 1;
    }
    pair->value = malloc(str_len * sizeof(char));
    if (pair->value == NULL) {
        return 1;
    }
    pair->value = strdup(val_ptr);
    pair->valuetype = STR;
    return 0;
}

// Resets pairs value to the given string
int restoraw(map_t *pair, char *val_ptr, int str_len) {
    if (pair == NULL) {
        return 1;
    }
    pair->value = malloc(str_len * sizeof(char));
    if (pair->value == NULL) {
        return 1;
    }
    pair->value = strdup(val_ptr);
    pair->valuetype = RAW;
    return 0;
}

// Resets pairs value to the given double
int restodouble(map_t *pair, double value) {
    if (pair == NULL) {
        return 1;
    }
    pair->value = malloc(sizeof(double));
    if (pair->value == NULL) {
        return 1;
    }
    *(double*)pair->value = value;
    pair->valuetype = DBL;
    return 0;
}

// Resets pairs value to the given long
int restolong(map_t *pair, long value) {
    if (pair == NULL) {
        return 1;
    }
    pair->value = malloc(sizeof(long));
    if (pair->value == NULL) {
        return 1;
    }
    *(long*)pair->value = value;
    pair->valuetype = LONG;
    return 0;
}

// Resets pairs value to the given long long
int restolonglong(map_t *pair, long long value) {
    if (pair == NULL) {
        return 1;
    }
    pair->value = malloc(sizeof(long long));
    if (pair->value == NULL) {
        return 1;
    }
    *(long long*)pair->value = value;
    pair->valuetype = LL;
    return 0;
}

// Resets pairs value to the given long double
int restolongdouble(map_t *pair, long double value) {
    if (pair == NULL) {
        return 1;
    }
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

// Gets the value in pair safely
void *getpairvalue(map_t *pair) {
    if (pair == NULL) {
        return NULL;
    }
    switch (pair->valuetype) {
        case RAW: return (char*)pair->value;
        case SHR: return (short*)pair->value;
        case STR: {
            char *str = malloc(strlen(pair->value)+3);
            if (str == NULL) {
                return NULL;
            }
            snprintf(str, strlen(pair->value)+3, "\"%s\"", (char*)pair->value);
            return str;
        }
        case INT: return (int*)pair->value;
        case FLT: return (float*)pair->value;
        case DBL: return (double*)pair->value;
        case LONG: return (long*)pair->value;
        case LL: return (long long*)pair->value;
        case LDBL: return (long double*)pair->value;
        default: return NULL;
    }
}

// Formats a pair to JSON file format
char *pairtoJSON(map_t *pair) {
    if (pair->key == NULL) {
        return NULL;
    }
    if (pair->value == NULL) {
        pair->valuetype = RAW;
        pair->value = "null";
    }
    int buffersize = strlen(pair->key);  
    buffersize += 6; // For the curly braces, ": " and the keys double quotes
    switch (pair->valuetype) {
        case RAW: buffersize += strlen(pair->value); break;
        case SHR: buffersize += 6; break;
        case STR: buffersize += strlen(pair->value)+2; break;
        case INT: buffersize += 11; break;
        case FLT: buffersize += 47; break;
        case DBL: buffersize += 100; break;
        case LONG: buffersize += 16; break;
        case LL: buffersize += 20; break;
        case LDBL: buffersize += 350; break;
        default: return NULL;
    }
    char *buffer = malloc(buffersize);
    if (buffer == NULL) {
        return NULL;
    }
    buffer[0] = '{';
    buffer[1] = '\0';
    strcat(buffer, "\"");
    strcat(buffer, pair->key);
    strcat(buffer, "\"");
    strcat(buffer, ": ");
    switch (pair->valuetype) {
        case RAW: strcat(buffer, (char*)pair->value); break;
        case SHR:{char srep[6]; // srep shortened for string representation
                  sprintf(srep, "%hd", *(short*)pair->value);
                  strcat(buffer, srep);
                  break;}
        case STR:{strcat(buffer, "\"");
                  strcat(buffer, (char*)pair->value);
                  strcat(buffer, "\"");
                  break;}
        case INT:{char srep[11];
                 sprintf(srep, "%d", *(int*)pair->value);
                 strcat(buffer, srep);
                 break;}
        case FLT:{char srep[47];
                 sprintf(srep, "%g", *(float*)pair->value);
                 strcat(buffer, srep);
                 break;}
        case DBL:{char srep[100];
                 sprintf(srep, "%lf", *(double*)pair->value);
                 strcat(buffer, srep);
                 break;}
        case LONG:{char srep[20];
                  sprintf(srep, "%ld", *(long*)pair->value);
                  strcat(buffer, srep);
                  break;}
        case LL:{char srep[20];
                sprintf(srep, "%lld", *(long long*)pair->value);
                strcat(buffer, srep);
                break;}
        case LDBL:{char srep[350];
                  sprintf(srep, "%Lf", *(long double*)pair->value);
                  strcat(buffer, srep);
                  break;}
        default: return NULL;
    }
    strcat(buffer, "}");
    return buffer;
}

int main() {

}