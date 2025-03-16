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

// Returns the length of the list
unsigned int listlen(list_t *list) {
    unsigned int len = 0;
    list_t *current = list;
    while (current != NULL) {
        len += 1;
        current = current->next;
    }
    return len;
}

int calclistsize(list_t *root) {
    if (root == NULL) {
        return -1;
    }
    int bsize = 0;
    list_t *current = root;
    while (current != NULL) {
        switch (current->valuetype) {
            case RAW: bsize += strlen(current->value);
            case SHR: bsize += SHR_STR_LEN; break;
            case INT: bsize += INT_STR_LEN; break;
            case FLT: bsize += FLT_STR_LEN; break;
            case DBL: bsize += DBL_STR_LEN; break;
            case LONG: bsize += LONG_STR_LEN; break;
            case LL: bsize += LL_STR_LEN; break;
            case LDBL: bsize += LDBL_STR_LEN; break;
            case LIST: bsize += calclistsize(current->value); break;
            default: return -2;
        }
        current = current->next;
    }
    return bsize;
}

// Returns the size of the hash map
int calcmapsize(map_t *root) {
    if (root == NULL) {
        return 1;
    }
    int bsize = 0;
    map_t *current = root;
    while (current != NULL) {
        bsize += strlen(current->key) + 2;
        switch (current->valuetype) {
            case RAW: bsize += strlen(current->value);
            case SHR: bsize += SHR_STR_LEN; break;
            case INT: bsize += INT_STR_LEN; break;
            case FLT: bsize += FLT_STR_LEN; break;
            case DBL: bsize += DBL_STR_LEN; break;
            case LONG: bsize += LONG_STR_LEN; break;
            case LL: bsize += LL_STR_LEN; break;
            case LDBL: bsize += LDBL_STR_LEN; break;
            case LIST: bsize += calclistsize(current->value); break;
            case NMAP: bsize += calcmapsize(current->value); break;
            default: return -2;
        }
        current = current->next;
    }
    return bsize;
}

// Forcefully frees a pair
int freepair(map_t *pair) {
    if (pair == NULL) {
        return 1;
    }
    if (pair->key != NULL) {
        free(pair->key);
    }
    if (pair->value != NULL) {
        free(pair->value);
    }
    free(pair);
    return 0;
}

// Forcefully frees an element
int free_element(list_t *element) {
    if (element == NULL) {
        return 1;
    }
    if (element->value != NULL) {
        free(element->value);
    }
    free(element);
    return 0;
}

// Frees all the pairs after map and itself
int freemap(map_t* map) {
    if (map == NULL) {
        return 1;
    }
    map_t *current = map;
    map_t *next;
    while (current != NULL) {
        next = current->next;
        freepair(current);
        current = next;
    }
    return 0;
}

// Frees the list (or frees every element after the given element)
int freelist(list_t *list) {
    if (list == NULL) {
        return 1;
    }
    list_t *current = list;
    list_t *next;
    while (current != NULL) {
        next = current->next;
        free_element(current);
        current = next;
    }
    return 0;
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

// Returns the element by index. For example if index is 1 it returns root->next
list_t *getelementbyindex(list_t *root, unsigned int index) {
    if (root == NULL) {
        return NULL;
    }
    if (index > listlen(root)) {
        return NULL;
    }
    list_t *current = root;
    int i;
    for (i = 0; i < index; i++) {
        current = current->next;
    }
    return current;
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
map_t *initM() {
    map_t* map = malloc(sizeof(map_t));
    if (map == NULL) {
        return NULL;
    }
    map->key = malloc(sizeof(char));
    map->value = malloc(sizeof(int));
    map->next = NULL;
    return map;
}

// Returns the last pair in a hash map
map_t *getlastpair(map_t *root) {
    if (root == NULL) {
        return NULL;
    }
    map_t *current = root;
    while (current->next !=  NULL) {
        current = current->next;
    }
    return current;
}

// Returns the last element in a list
list_t *getlastelement(list_t *root) {
    if (root == NULL) {
        return NULL;
    }
    list_t *current = root;
    while (current->next != NULL) {
        current = current->next;
    }
    return current;
}

// Initializes a list
list_t *initL() {
    list_t *list = malloc(sizeof(list_t));
    if (list == NULL) {
        return NULL;
    }
    list->value = malloc(sizeof(int));
    list->valuetype = RAW;
    list->next = NULL;
    return list;
}

// Adds a pair to the end of the given map and returns it
map_t *appendH(map_t *map) {
    if (map == NULL) {
        return NULL;
    }
    map_t *current = map;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = initializemap();
    if (current->next == NULL) {
        return NULL;
    }
    return current->next;
}

// Adds an element to the end of the given list and returns it
list_t *appendL(list_t *list) {
    if (list == NULL) {
        return NULL;
    }
    list_t *current = list;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = initializelist();
    if (current->next == NULL) {
        return NULL;
    }
    return current->next;
}

// Adds another pair after the given pair and returns it
map_t *addafterH(map_t *pair) {
    if (pair == NULL) {
        return NULL;
    }
    map_t *new = initializemap();
    if (pair->next != NULL) {
        new->next = pair->next;
    }
    pair->next = new;
    return new;
}

// Adds another element after the given element and returns it
list_t *addafterL(list_t *element) {
    if (element == NULL) {
        return NULL;
    }
    list_t *new = initializelist();
    if (element->next != NULL) {
        new->next = element->next;
    }
    element->next = new;
    return new;
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

// Removes the last element in a list
int removelastL(list_t *list) {
    if (list == NULL) {
        return 1;
    }
    if (list->next == NULL) {
        free_element(list);
        list = NULL;
        return 0;
    }
    list_t *current = list;
    while (current->next != NULL) {
        current = current->next;
    }
    free_element(current);
    current = NULL;
    return 0;
}

// Removes the given map and then reassigns the next pointed the pair before the given map to pair after the given map
int removeafterH(map_t *root, map_t *pairtormv) {
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

// Removes an element after the given element
int removeafterL(list_t *root) {
    if (root == NULL) {
        return 1;
    }
    if (root->next == NULL) {
        free_element(root);
        root = NULL;
        return 0;
    } else {
        list_t *temp = root->next;
        root->next = temp->next;
        free_element(temp);
        return 0;
    }
}

// Resets pairs key to the given string
map_t *resetkey(map_t *pair, char *key, unsigned int str_len) {
    if (pair == NULL) {
        return NULL;
    }
    free(pair->key);
    pair->key = malloc(str_len + 1);
    if (pair->key == NULL) {
        return NULL;
    }
    strncpy(pair->key, key, str_len);
    pair->key[str_len] = '\0';
    return pair;
}

/* Note about the last functions: 1 return value represents NULL error, indicates if map_t or list_t is null &
   2 return value represents memory allocation error
*/

// Resets pairs value to the given integer
int setintH(map_t *pair, int value) {
    if (pair == NULL) {
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(int));
    if (pair->value == NULL) {
        return 2;
    }
    *(int*)pair->value = value;
    pair->valuetype = INT;
    return 0;
}

int setintL(list_t *element, int value) {
    if (element == NULL) {
        return 1;
    }
    free(element->value);
    element->value = malloc(sizeof(int));
    if (element->value == NULL) {
        return 2;
    }
    *(int*)element->value = value;
    element->valuetype = INT;
    return 0;
}

// Resets pairs value to the given short
int setshortH(map_t *pair, short value) {
    if (pair == NULL) {
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(short));
    if (pair->value == NULL) {
        return 2;
    }
    *(short*)pair->value = value;
    pair->valuetype = SHR;
    return 0;
}

int setshortL(list_t *element, short value) {
    if (element == NULL) {
        return 1;
    }
    free(element->value);
    element->value = malloc(sizeof(short));
    if (element->value == NULL) {
        return 2;
    }
    *(short*)element->value = value;
    element->valuetype = SHR;
    return 0;
}

// Resets pairs value to the given float
int setfloatH(map_t *pair, float value) {
    if (pair == NULL) {
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(float));
    if (pair->value == NULL) {
        return 2;
    }
    *(float*)pair->value = value;
    pair->valuetype = FLT;
    return 0;
}

int setfloatL(list_t *element, float value) {
    if (element == NULL) {
        return 1;
    }
    free(element->value);
    element->value = malloc(sizeof(float));
    if (element->value == NULL) {
        return 2;
    }
    *(float*)element->value = value;
    element->valuetype = FLT;
    return 0;
}

// Resets pairs value to the given string
int setrawH(map_t *pair, char *val_ptr, int str_len) {
    if (pair == NULL) {
        return 1;
    }
    free(pair->value);
    pair->value = malloc(str_len + 1);
    if (pair->value == NULL) {
        return 2;
    }
    strncpy(pair->value, val_ptr, str_len);
    ((char*)pair->value)[str_len] = '\0';
    pair->valuetype = RAW;
    return 0;
}

int setrawL(list_t *element, char *val_ptr, unsigned int str_len) {
    if (element == NULL) {
        return 1;
    }
    free(element->value);
    element->value = malloc(str_len + 1);
    if (element->value == NULL) {
        return 2;
    }
    strncpy(element->value, val_ptr, str_len);
    ((char*)element->value)[str_len] = '\0';
    element->valuetype = RAW;
    return 0;
}

// Resets pairs value to the given double
int setdoubleH(map_t *pair, double value) {
    if (pair == NULL) {
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(double));
    if (pair->value == NULL) {
        return 2;
    }
    *(double*)pair->value = value;
    pair->valuetype = DBL;
    return 0;
}

int setdoubleL(list_t *element, double value) {
    if (element == NULL) {
        return 1;
    }
    free(element->value);
    element->value = malloc(sizeof(value));
    if (element->value == NULL) {
        return 2;
    }
    *(double*)element->value = value;
    element->valuetype = DBL;
    return 0;
}

// Resets pairs value to the given long
int setlongH(map_t *pair, long value) {
    if (pair == NULL) {
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(long));
    if (pair->value == NULL) {
        return 2;
    }
    *(long*)pair->value = value;
    pair->valuetype = LONG;
    return 0;
}

int setlongL(list_t *element, long value) {
    if (element == NULL) {
        return 1;
    }
    free(element->value);
    element->value = malloc(sizeof(long));
    if (element->value == NULL) {
        return 2;
    }
    *(long*)element->value = value;
    element->valuetype = LONG;
    return 0;
}

// Resets pairs value to the given long long
int setlonglongH(map_t *pair, long long value) {
    if (pair == NULL) {
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(long long));
    if (pair->value == NULL) {
        return 2;
    }
    *(long long*)pair->value = value;
    pair->valuetype = LL;
    return 0;
}

int setlonglongL(list_t *element, long long value) {
    if (element == NULL) {
        return 1;
    }
    free(element->value);
    element->value = malloc(sizeof(long long));
    if (element->value == NULL) {
        return 2;
    }
    *(long long*)element->value = value;
    element->valuetype = LL;
    return 0;
}


// Resets pairs value to the given long double
int setlongdoubleH(map_t *pair, long double value) {
    if (pair == NULL) {
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(long double));
    if (pair->value == NULL) {
        return 2;
    }
    *(long double*)pair->value = value;
    pair->valuetype = LDBL;
    return 0;
}

int setlongdoubleL(list_t *element, long double value) {
    if (element == NULL) {
        return 1;
    }
    free(element->value);
    element->value = malloc(sizeof(long double));
    if (element->value == NULL) {
        return 2;
    }
    *(long double*)element->value = value;
    element->valuetype = LDBL;
    return 0;
}

// Resets pairs value to the given list
int setlistH(map_t *pair, list_t *element) {
    if (pair == NULL || element == NULL) {
        return 1;
    }
    free(pair->value);
    pair->value = element;
    pair->valuetype = LIST;
    return 0;
}


int setlistL(list_t *element, list_t *e2) {
    if (element == NULL) {
        return 1;
    }
    free(element->value);
    element->value = e2;
    element->valuetype = LIST;
    return 0;
}

// Resets pairs value to the given hashmap
int setmapH(map_t *pair, map_t *p2) {
    if (pair == NULL || p2 == NULL) {
        return 1;
    }
    free(pair->value);
    pair->value = p2;
    pair->valuetype = NMAP;
    return 0;
}

int setmapL(list_t *element, map_t *map) {
    if (element == NULL || map == NULL) {
        return 1;
    }
    free(element->value);
    element->value = map;
    element->valuetype = NMAP;
}