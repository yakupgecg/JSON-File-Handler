// MAIN IDEA: Make this safe enough for basic JSON file formatting

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RAW -1 // For making a string without double quotes
#define SHR 0
#define STR 1
#define INT 2
#define FLT 3
#define DBL 4
#define LONG 5
#define LL 6
#define LDBL 7

typedef struct Hmap {
    const char *key;
    void *value;
    int valuetype;
    struct Hmap *next;
} map_t;

// This will initalize a map and then return a pointer to it.
map_t *initalizemap() {
    map_t* var1 = malloc(sizeof(map_t));
    var1->next = NULL;
    return var1;
}

// This will replace the maps key and value and valuetype with a new one
int resetpair(map_t* hm, char *key, void *value, int valuetype) {
    if (hm == NULL) {
        return 1;
    }
    hm->key = strdup(key);
    hm->value = value;
    hm->valuetype = valuetype;
    hm->next = NULL;
    return 0;
}

// This will add a new pair to the end
int addpair(map_t *hm, char *key, void *value, int valuetype) {
    map_t* current = hm;
    if (hm == NULL) {
        return 1;
    }
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = initalizemap();
    current->next->key = strdup(key);
    current->next->value = value;
    current->next->valuetype = valuetype;
    current->next->next = NULL;
    return 0;
}

// Next: Add function to add pairs after another

// This will delete the last pair the map
int deletelast(map_t *hm) {
    map_t *current = hm;
    if (current->next == NULL) {
        free(current);
        return 0;
    }
    while (current->next != NULL) {
        if (current->next->next == NULL) {
            free(current->next);
            current->next = NULL;
            return 0;
        }
        current = current->next;
    }
    return 1;
}

// This will get a map out by name. If hm is the base of all the other pairs in that specific map, the next pair of hm is to be classified as the base.
int deletebykey(map_t *hm, const char *keytofind) {
    // Next: Finish the function
    map_t *current = hm;
    map_t *prev = NULL;

    if (current->next = NULL) {
        
    }

    while (current != NULL) {
        if (strcmp(current->key, keytofind) == 0) {
            free(current);
            return 0;
        }
        current = current->next;
    }
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
        free(current);
        current = next;
    }
}

// For translating hashmaps to JSON file format
char *mapToJson(map_t *hm, size_t size) {
    // Next: Add automatic buffersizer
    map_t *current = hm;
    char *buffer = malloc(size);
    if (buffer == NULL) {
        return NULL;
    }
    buffer[0] = '\0'; 
    strcat(buffer, "{");
    while (current != NULL) {
        char *unmd1 = malloc(4392);
        sprintf(unmd1, "\"%s\": ", current->key);
        strcat(buffer, unmd1);
        switch (current->valuetype) {
            case INT:
                sprintf(unmd1, "%d", *(int*)current->value);
                strcat(buffer, unmd1);
                free(unmd1);
                break;
            case FLT:
                sprintf(unmd1, "%f", *(float*)current->value);
                strcat(buffer, unmd1);
                free(unmd1);
                break;
            case DBL:
                sprintf(unmd1, "%lf", *(double*)current->value);
                strcat(buffer, unmd1);
                free(unmd1);
                break;
            case LONG:
                sprintf(unmd1, "%ld", *(long*)current->value);
                strcat(buffer, unmd1);
                free(unmd1);
                break;
            case LL:
                sprintf(unmd1, "%lld", *(long long*)current->value);
                strcat(buffer, unmd1);
                free(unmd1);
                break;
            case LDBL:
                sprintf(unmd1, "%Lf", *(long double*)current->value);
                strcat(buffer, unmd1);
                free(unmd1);
                break;
            case STR:
                sprintf(unmd1, "\"%s\"", (char*)current->value);
                strcat(buffer, unmd1);
                free(unmd1);
                break;
            case SHR:
                sprintf(unmd1, "%hd", *(short*)current->value);
                strcat(buffer, unmd1);
                free(unmd1);
                break;
            case RAW:
                sprintf(unmd1, "%s", *(char*)current->value);
                strcat(buffer, unmd1);
                free(unmd1);
            default:
                sprintf(unmd1, "\"<Unknown type>\"");
                strcat(buffer, unmd1);
                free(unmd1);
                break;
        }
        current = current->next;
        if (current != NULL) {
            sprintf(unmd1, ", ");
            strcat(buffer, unmd1);
        }
    }
    strcat(buffer, "}");
    return buffer;
}

// For translating a pair into JSON file format
char *pairToJson(map_t *hm, size_t size) {
    char *json = malloc(size);
    if (json == NULL) {
        return NULL;
    }
    json[0] = '\0';
    strcat(json, "{");
    char *buf = malloc(500);
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