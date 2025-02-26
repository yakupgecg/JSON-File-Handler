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

// This will replace the hashmaps key and value and valuetype with a new one
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

// This will add a new pair to the end of the hashmap
int addlast(map_t *hm, char *key, void *value, int valuetype) {
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

// Adds a pair after another pair in a hashmap
void addafter(map_t *hm, char *key, void *value, int valuetype) {
    map_t *hmnext = hm->next;
    map_t *newmap = initalizemap();
    newmap->key = strdup(key);
    newmap->value = value;
    newmap->valuetype = valuetype;
    hm->next = newmap;
    newmap->next = hmnext;
}


// This will delete the last pair of the hashmap
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

// This will get a hashmap out by name. If hm is the base of all the other pairs in that specific hashmap, the next pair of hm is to be classified as the base.
int deletebykey(map_t *hm, const char *keytofind) {
    map_t *current = hm;
    map_t *prev = NULL;

    if (strcmp(current->key, keytofind) == 0) {
        prev = current;
        current = current->next;
        free(prev);
        return 0;
    }
    prev = hm;
    while (current != NULL) {
        if (strcmp(current->key, keytofind) == 0) {
            prev->next = current->next;
            free(current);
            return 0;
        }
        prev = current;
        current = current->next;
    }
    return 1;
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
char *pairToJson(map_t *hm, size_t size) {
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