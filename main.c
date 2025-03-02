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
    map_t* var1 = malloc(sizeof(map_t));
    var1->next = NULL;
    return var1;
}

// Next: Add a resto functions for every datatype like a restoint or restostring function

int resetkey(map_t *pair, char *key) {
    free(pair->key);
    pair->key = strdup(key);
    if (pair->key == NULL) {
        return 1;
    }
    return 0;
}

int restoint(map_t *pair, int value) {
    if (pair->value == NULL) {
        pair->value = malloc(sizeof(int));
        if (pair->value == NULL) {
            return 1;
        }
    }
    *(int*)pair->value = value;
    pair->valuetype = INT;
    return 0;
}

int restoshort(map_t *pair, short value) {
    if (pair->value == NULL) {
        pair->value = malloc(sizeof(short));
        if (pair->value == NULL) {
            return 1;
        }
    }
    *(short*)pair->value = value;
    pair->valuetype = SHR;
    return 0;
}

int restofloat(map_t *pair, float value) {
    if (pair->value == NULL) {
        pair->value = malloc(sizeof(float));
        if (pair->value == NULL) {
            return 1;
        }
    }
    *(float*)pair->value = value;
    pair->valuetype = FLT;
    return 0;
}

int restostring(map_t *pair, char *val_ptr, int str_len) {
    if (pair->value == NULL) {
        pair->value = malloc(str_len * sizeof(char));
        if (pair->value == NULL) {
            return 1;
        }
    }
    *(char*)pair->value = val_ptr;
    pair->valuetype = STR;
    return 0;
}

int restodouble(map_t *pair, double value) {
    if (pair->value == NULL) {
        pair->value = malloc(sizeof(double));
        if (pair->value == NULL) {
            return 1;
        }
    }
    *(double*)pair->value = value;
    pair->valuetype = DBL;
    return 0;
}

int restolong(map_t *pair, long value) {
    if (pair->value == NULL) {
        pair->value = malloc(sizeof(long));
        if (pair->value == NULL) {
            return 1;
        }
    }
    *(long*)pair->value = value;
    pair->valuetype = LONG;
    return 0;
}

int restolonglong(map_t *pair, long long value) {
    if (pair->value == NULL) {
        pair->value = malloc(sizeof(long long));
        if (pair->value == NULL) {
            return 1;
        }
    }
    *(long long*)pair->value = value;
    pair->valuetype = LL;
    return 0;
}

int restolongdouble(map_t *pair, long double value) {
    if (pair->value == NULL) {
        pair->value = malloc(sizeof(long double));
        if (pair->value == NULL) {
            return 1;
        }
    }
    *(long double*)pair->value = value;
    pair->valuetype = LDBL;
    return 0;
}
 
// Calculates the length of the hashmap, but if the pair is not the root of the hashmap, it will start from pair
size_t maplen(map_t *hm) {
    size_t len = 0;
    map_t *current = hm;
    while (current != NULL) {
        len += 1;
        current = current->next;
    }
    return len;
}

void freemap(map_t* hm) {
    map_t* current = hm;
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

void freepair(map_t *pair) {
    if (pair->key != NULL) {
        free(pair->key);
    }
    if (pair->value != NULL) {
        free(pair->value);
    }
    free(pair);
}

// For translating hashmaps to JSON file format
char *mapToJson(map_t *hm, size_t size) {
    map_t *current = hm;
    int buffersize = 50; // Buffersize is a bit bigger for backup memory just in case
    while (current != NULL) {
        buffersize += 2 // For the , and space
        buffersize += strlen(current->key) + 2;
        switch (current->valuetype) {
            case INT:
                buffersize += 11;
                break;
            case FLT:
                buffersize += 47;
                break;
            case DBL:
                buffersize += 325;
                break;
            case LONG:
                buffersize += 15;
            case LL:
                buffersize += 20
                break;
            case LDBL:
                buffersize += 4932;
                break;
            case STR:
                buffersize += strlen(current->value) + 2;
                break;
            case SHR:
                buffersize += 6;
                break;
            case RAW:
                buffersize += strlen(current->value);
            default:
                buffersize += 10;
                break;
        }
    }
    current = hm;
    char *buffer = malloc(buffersize);
    if (buffer == NULL) {
        return NULL;
    }
    buffer[0] = '\0'; 
    strcat(buffer, "{");
    while (current != NULL) {
        char *strtocpy = malloc(1000);
        sprintf(strtocpy, "\"%s\": ", current->key);
        strcat(buffer, strtocpy);
        switch (current->valuetype) {
            case INT:
                sprintf(strtocpy, "%d", *(int*)current->value);
                strcat(buffer, strtocpy);
                free(strtocpy);
                break;
            case FLT:
                sprintf(strtocpy, "%f", *(float*)current->value);
                strcat(buffer, strtocpy);
                free(strtocpy);
                break;
            case DBL:
                sprintf(strtocpy, "%lf", *(double*)current->value);
                strcat(buffer, strtocpy);
                free(strtocpy);
                break;
            case LONG:
                sprintf(strtocpy, "%ld", *(long*)current->value);
                strcat(buffer, strtocpy);
                free(strtocpy);
                break;
            case LL:
                sprintf(strtocpy, "%lld", *(long long*)current->value);
                strcat(buffer, strtocpy);
                free(strtocpy);
                break;
            case LDBL:
                sprintf(strtocpy, "%Lf", *(long double*)current->value);
                strcat(buffer, strtocpy);
                free(strtocpy);
                break;
            case STR:
                sprintf(strtocpy, "\"%s\"", (char*)current->value);
                strcat(buffer, strtocpy);
                free(strtocpy);
                break;
            case SHR:
                sprintf(strtocpy, "%hd", *(short*)current->value);
                strcat(buffer, strtocpy);
                free(strtocpy);
                break;
            case RAW:
                sprintf(strtocpy, "%s", *(char*)current->value);
                strcat(buffer, strtocpy);
                free(strtocpy);
            default:
                sprintf(strtocpy, "\"<Unknown type>\"");
                strcat(buffer, strtocpy);
                free(strtocpy);
                break;
        }
        current = current->next;
        if (current != NULL) {
            sprintf(strtocpy, ", ");
            strcat(buffer, strtocpy);
        }
    }
    strcat(buffer, "}");
    return buffer;
}

// For translating a pair into JSON file format
char *pairToJson(map_t *hm) {
    int buffersize = 20;
    buffersize += 2 // For the , and space
    buffersize += strlen(current->key) + 2;
    switch (hm->valuetype) {
        case INT:
            buffersize += 11;
            break;
        case FLT:
            buffersize += 47;
            break;
        case DBL:
            buffersize += 325;
            break;
        case LONG:
            buffersize += 15;
        case LL:
            buffersize += 20
            break;
        case LDBL:
            buffersize += 4932;
            break;
        case STR:
            buffersize += strlen(current->value) + 2;
            break;
        case SHR:
            buffersize += 6;
            break;
        case RAW:
            buffersize += strlen(current->value);
        default:
            buffersize += 10;
            break;
    }
    char *json = malloc(size);
    if (json == NULL) {
        return NULL;
    }
    json[0] = '\0';
    strcat(json, "{");
    char *buf = malloc(1000);
    sprintf(buf, "\"%s\": ", hm->key);
    strcat(json, buf);
    switch (hm->valuetype) {
        case INT:
            sprintf(buf, "%d}", *(int*)hm->value);
            strcat(json, buf);
            break;
        case FLT: 
            sprintf(buf, "%f}", *(float*)hm->value);
            strcat(json, buf);
            break;
        case DBL:
            sprintf(buf, "%lf}", *(double*)hm->value);
            strcat(json, buf);
            break;
        case LONG:
            sprintf(buf, "%ld}", *(long*)hm->value);
            strcat(json, buf);
            break;
        case LL:
            sprintf(buf, "%lld}", *(long long*)hm->value);
            strcat(json, buf);
            break;
        case LDBL:
            sprintf(buf, "%Lf}", *(long double*)hm->value);
            strcat(json, buf);
            break;
        case STR:
            sprintf(buf, "\"%s\"}", (char*)hm->value);
            strcat(json, buf);
            break;
        case SHR:
            sprintf(buf, "%hd}", *(short*)hm->value);
            strcat(json, buf);
            break;
        case RAW:
            sprintf(buf, "%s}", (char*)hm->value);
            strcat(json, buf);
            break;
        default:
            sprintf(buf, "\"<Unknown type>\"}");
            strcat(json, buf);
            break;
    }
    free(buf);
    return json;
}

int main() {

}