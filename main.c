// MAIN IDEA: Make this safe enough for basic JSON file formatting

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RAW 0 // Raw (For example a string, or a null in JSON)
#define SHR 1 // Short
#define INT 2 // Integer
#define FLT 3 // Float
#define DBL 4 // Double
#define LONG 5 // Long
#define LL 6 // Long long
#define LDBL 7  // Long Double

// Every valuetypes string (representation) length can be customized
int SHR_STR_LEN = 6;
int INT_STR_LEN = 11;
int FLT_STR_LEN = 47;
int DBL_STR_LEN = 100;
int LONG_STR_LEN = 20;
int LL_STR_LEN = 20;
int LDBL_STR_LEN = 350;

typedef struct Hmap {
    char *key;
    void *value;
    int valuetype;
    struct Hmap *next;
} map_t;

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

int addafter(map_t *map) {
    if (map == NULL) {
        return 1;
    }
    map_t *new = initializemap();
    if (map->next != NULL) {
        new->next = map->next;
    }
    map->next = new;
    return 0;
}

// Removes the last pair in a hashmap
int removelast(map_t *map) {
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
int restoshort(map_t *pair, short value) {
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
int restofloat(map_t *pair, float value) {
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
int restoraw(map_t *pair, char *val_ptr, int str_len) {
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
int restodouble(map_t *pair, double value) {
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
int restolong(map_t *pair, long value) {
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
int restolonglong(map_t *pair, long long value) {
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
int restolongdouble(map_t *pair, long double value) {
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
        case SHR: buffersize += SHR_STR_LEN; break;
        case INT: buffersize += INT_STR_LEN; break;
        case FLT: buffersize += FLT_STR_LEN; break;
        case DBL: buffersize += DBL_STR_LEN; break;
        case LONG: buffersize += LONG_STR_LEN; break;
        case LL: buffersize += LL_STR_LEN; break;
        case LDBL: buffersize += LDBL_STR_LEN; break;
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
        case SHR:{char srep[SHR_STR_LEN]; // srep shortened for string representation
                  sprintf(srep, "%hd", *(short*)pair->value);
                  strcat(buffer, srep);
                  break;}
        case INT:{char srep[INT_STR_LEN];
                 sprintf(srep, "%d", *(int*)pair->value);
                 strcat(buffer, srep);
                 break;}
        case FLT:{char srep[FLT_STR_LEN];
                 sprintf(srep, "%g", *(float*)pair->value);
                 strcat(buffer, srep);
                 break;}
        case DBL:{char srep[DBL_STR_LEN];
                 sprintf(srep, "%lf", *(double*)pair->value);
                 strcat(buffer, srep);
                 break;}
        case LONG:{char srep[LONG_STR_LEN];
                  sprintf(srep, "%ld", *(long*)pair->value);
                  strcat(buffer, srep);
                  break;}
        case LL:{char srep[LL_STR_LEN];
                sprintf(srep, "%lld", *(long long*)pair->value);
                strcat(buffer, srep);
                break;}
        case LDBL:{char srep[LDBL_STR_LEN];
                  sprintf(srep, "%Lf", *(long double*)pair->value);
                  strcat(buffer, srep);
                  break;}
        default: return NULL;
    }
    strcat(buffer, "}");
    return buffer;
}

// Convert a hash map structure to a JSON string
char *maptoJSON(map_t *map, unsigned int buffersize) {
    if (map == NULL) {
        return NULL;
    }
    char *buffer = malloc(buffersize + 1);
    if (buffer == NULL) {
        return NULL;
    }
    buffer[0] = '{';
    buffer[1] = '\0';
    map_t *current = map;
    while (current != NULL) {
        strcat(buffer, "\"");
        strcat(buffer, current->key);
        strcat(buffer, "\"");
        strcat(buffer, ": ");
        switch (current->valuetype) {
            case RAW: strcat(buffer, (char*)current->value); break;
            case SHR:{char srep[SHR_STR_LEN]; // srep shortened for string representation
                      sprintf(srep, "%hd", *(short*)current->value);
                      strcat(buffer, srep);
                      break;}
            case INT:{char srep[INT_STR_LEN];
                     sprintf(srep, "%d", *(int*)current->value);
                     strcat(buffer, srep);
                     break;}
            case FLT:{char srep[FLT_STR_LEN];
                     sprintf(srep, "%g", *(float*)current->value);
                     strcat(buffer, srep);
                     break;}
            case DBL:{char srep[DBL_STR_LEN];
                     sprintf(srep, "%lf", *(double*)current->value);
                     strcat(buffer, srep);
                     break;}
            case LONG:{char srep[LONG_STR_LEN];
                      sprintf(srep, "%ld", *(long*)current->value);
                      strcat(buffer, srep);
                      break;}
            case LL:{char srep[LL_STR_LEN];
                    sprintf(srep, "%lld", *(long long*)current->value);
                    strcat(buffer, srep);
                    break;}
            case LDBL:{char srep[LDBL_STR_LEN];
                      sprintf(srep, "%Lf", *(long double*)current->value);
                      strcat(buffer, srep);
                      break;}
            default: return NULL;
        }
        current = current->next;
        if (current != NULL) {
            strcat(buffer, ", ");
        }
    }
    strcat(buffer, "}");
    return buffer;
}

int main() {

}