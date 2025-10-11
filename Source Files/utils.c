#include "..\Headers\utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Returns the length of the hashmap, but if the pair is not the root of the hashmap, it will start from pair
unsigned int map_len(obj_t *map) {
    unsigned int len = 0;
    obj_t *current = map;
    while (current != NULL) {
        len += 1;
        current = current->next;
    }
    return len;
}

// Returns the length of the list
unsigned int list_len(array_t *list) {
    unsigned int len = 0;
    array_t *current = list;
    while (current != NULL) {
        len += 1;
        current = current->next;
    }
    return len;
}

// Calculates the length of the given array
int list_size(array_t *root) {
    if (root == NULL) {
        errno = EINVAL;
        return -1;
    }
    int bsize = 0;
    array_t *current = root;
    while (current != NULL) {
        switch (current->valuetype) {
            case RAW: bsize += strlen(current->value); break;
            case SHR: bsize += SHR_STR_LEN; break;
            case INT: bsize += INT_STR_LEN; break;
            case FLT: bsize += FLT_STR_LEN; break;
            case DBL: bsize += DBL_STR_LEN; break;
            case LONG: bsize += LONG_STR_LEN; break;
            case LL: bsize += LL_STR_LEN; break;
            case LDBL: bsize += LDBL_STR_LEN; break;
            case LIST: bsize += list_size(current->value); break;
            default: errno = EINVAL; return -2;
        }
        current = current->next;
    }
    return bsize;
}

// Returns the size of the hash map
int map_size(obj_t *root) {
    if (root == NULL) {
        errno = EINVAL;
        return -1;
    }
    int bsize = 0;
    obj_t *current = root;
    while (current != NULL) {
        bsize += strlen(current->key) + 2;
        switch (current->valuetype) {
            case RAW: bsize += strlen(current->value); break;
            case SHR: bsize += SHR_STR_LEN; break;
            case INT: bsize += INT_STR_LEN; break;
            case FLT: bsize += FLT_STR_LEN; break;
            case DBL: bsize += DBL_STR_LEN; break;
            case LONG: bsize += LONG_STR_LEN; break;
            case LL: bsize += LL_STR_LEN; break;
            case LDBL: bsize += LDBL_STR_LEN; break;
            case LIST: bsize += list_size(current->value); break;
            case NMAP: bsize += map_size(current->value); break;
            default: errno = EINVAL; return -2;
        }
        current = current->next;
    }
    return bsize;
}

// Forcefully frees a pair
int free_pair(obj_t *pair) {
    if (pair == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (pair->key != NULL) {
        free(pair->key);
    }
    if (pair->value != NULL) {
        free(pair->value);
    }
    if (pair->valuetype == NMAP) {
        free_map(pair->value);
    } else if (pair->valuetype == LIST) {
        free_list(pair->value);
    }
    free(pair);
    return 0;
}

// Forcefully frees an element
int free_element(array_t *element) {
    if (element == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (element->value != NULL) {
        free(element->value);
    }
    if (element->valuetype == NMAP) {
        free_map(element->value);
    } else if (element->valuetype == LIST) {
        free_list(element->value);
    }
    free(element);
    return 0;
}

// Frees all the pairs after map and itself
int free_map(obj_t* map) {
    if (map == NULL) {
        errno = EINVAL;
        return 1;
    }
    obj_t *current = map;
    obj_t *next;
    while (current != NULL) {
        next = current->next;
        free_pair(current);
        current = next;
    }
    return 0;
}

// Frees the list (or frees every element after the given element)
int free_list(array_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return 1;
    }
    array_t *current = list;
    array_t *next;
    while (current != NULL) {
        next = current->next;
        free_element(current);
        current = next;
    }
    return 0;
}

// Returns the hash map, which has the key to find
obj_t *pairbykey(obj_t *root, char *key) {
    if (root == NULL) {
        errno = EINVAL;
        return NULL;
    }
    obj_t *current = root;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Returns the element by index. For example if index is 1 it returns root->next
array_t *getelementbyindex(array_t *root, unsigned int index) {
    if (root == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (index > list_len(root)) index = list_len(root);
    array_t *current = root;
    int i;
    for (i = 0; i < index; i++) {
        current = current->next;
    }
    return current;
}

// Returns the previous pair before the pair that has the key to find, if found
obj_t *pr_pairbykey(obj_t *root, char *key) {
    if (root == NULL) {
        errno = EINVAL;
        return NULL;
    }
    obj_t *prev = root;
    if (strcmp(prev->key, key) == 0) {
        return prev;
    }
    obj_t *current = root->next;
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
obj_t *initM() {
    obj_t* map = malloc(sizeof(obj_t));
    if (map == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    map->key = malloc(sizeof(char));
    if (map->key == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    map->value = malloc(sizeof(int));
    if (map->value == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    map->valuetype = RAW;
    map->next = NULL;
    return map;
}

// Returns the last pair in a hash map
obj_t *last_pair(obj_t *root) {
    if (root == NULL) {
        errno = EINVAL;
        return NULL;
    }
    obj_t *current = root;
    while (current->next !=  NULL) {
        current = current->next;
    }
    return current;
}

// Returns the last element in a list
array_t *last_element(array_t *root) {
    if (root == NULL) {
        errno = EINVAL;
        return NULL;
    }
    array_t *current = root;
    while (current->next != NULL) {
        current = current->next;
    }
    return current;
}

// Initializes a list
array_t *initL() {
    array_t *list = malloc(sizeof(array_t));
    if (list == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    list->value = malloc(sizeof(int));
    if (list->value == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    list->valuetype = RAW;
    list->next = NULL;
    return list;
}

// Adds a pair to the end of the given map and returns it
obj_t *appendH(obj_t *map) {
    if (map == NULL) {
        errno = EINVAL;
        return NULL;
    }
    obj_t *current = map;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = initM();
    if (current->next == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    return current->next;
}

// Adds an element to the end of the given list and returns it
array_t *appendL(array_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return NULL;
    }
    array_t *current = list;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = initL();
    if (current->next == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    return current->next;
}

// Adds another pair after the given pair and returns it
obj_t *insertH(obj_t *pair) {
    if (pair == NULL) {
        errno = EINVAL;
        return NULL;
    }
    obj_t *new = initM();
    if (pair->next != NULL) {
        new->next = pair->next;
    }
    pair->next = new;
    return new;
}

// Adds another element after the given element and returns it
array_t *insertL(array_t *element) {
    if (element == NULL) {
        errno = EINVAL;
        return NULL;
    }
    array_t *new = initL();
    if (element->next != NULL) {
        new->next = element->next;
    }
    element->next = new;
    return new;
} 

// Removes the last pair in a hashmap
int r_lastH(obj_t *map) {
    if (map == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (map->next == NULL) {
        free_pair(map);
        return 0;
    }
    obj_t *current = map;
    while (current->next->next != NULL) {
        current = current->next;
    }
    free_pair(current->next);
    current->next = NULL;
    return 0;
}

// Removes the last element in a list
int r_lastL(array_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (list->next == NULL) {
        free_element(list);
        return 0;
    }
    array_t *current = list;
    while (current->next->next != NULL) {
        current = current->next;
    }
    free_element(current->next);
    current->next = NULL;
    return 0;
}

// Removes the given map and then reassigns the next pointed the pair before the given map to pair after the given map
int r_afterH(obj_t *root, obj_t *pairtormv) {
    if (root == NULL) {
        errno = EINVAL;
        return 1;
    }
    obj_t *current = root;
    while (current != NULL) {
        if (strcmp(current->key, pairtormv->key) == 0) {
            pr_pairbykey(root, pairtormv->key)->next = current->next;
            free_pair(current);
            return 0;
        }
        current = current->next;
    }
    return 0;
}

// Removes an element after the given element
int r_afterL(array_t *root) {
    if (root == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (root->next == NULL) {
        free_element(root);
        root = NULL;
        return 0;
    } else {
        array_t *temp = root->next;
        root->next = temp->next;
        free_element(temp);
        return 0;
    }
}

// Resets pairs key to the given string
obj_t *resetkey(obj_t *pair, char *key, unsigned int str_len) {
    if (pair == NULL) {
        errno = EINVAL;
        return NULL;
    }
    free(pair->key);
    pair->key = malloc(str_len + 1);
    if (pair->key == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    strncpy(pair->key, key, str_len);
    pair->key[str_len] = '\0';
    return pair;
}

/* Note about the last functions: 1 return value represents NULL error, indicates if obj_t or array_t is null &
   2 return value represents memory allocation error
*/

// Resets pairs value to the given integer
int setintH(obj_t *pair, int value) {
    if (pair == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(int));
    if (pair->value == NULL) {
        errno = ENOMEM;
        return 2;
    }
    *(int*)pair->value = value;
    pair->valuetype = INT;
    return 0;
}

int setintL(array_t *element, int value) {
    if (element == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(element->value);
    element->value = malloc(sizeof(int));
    if (element->value == NULL) {
        errno = ENOMEM;
        return 2;
    }
    *(int*)element->value = value;
    element->valuetype = INT;
    return 0;
}

// Resets pairs value to the given short
int setshortH(obj_t *pair, short value) {
    if (pair == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(short));
    if (pair->value == NULL) {
        errno = ENOMEM;
        return 2;
    }
    *(short*)pair->value = value;
    pair->valuetype = SHR;
    return 0;
}

int setshortL(array_t *element, short value) {
    if (element == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(element->value);
    element->value = malloc(sizeof(short));
    if (element->value == NULL) {
        errno = ENOMEM;
        return 2;
    }
    *(short*)element->value = value;
    element->valuetype = SHR;
    return 0;
}

// Resets pairs value to the given float
int setfloatH(obj_t *pair, float value) {
    if (pair == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(float));
    if (pair->value == NULL) {
        errno = ENOMEM;
        return 2;
    }
    *(float*)pair->value = value;
    pair->valuetype = FLT;
    return 0;
}

int setfloatL(array_t *element, float value) {
    if (element == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(element->value);
    element->value = malloc(sizeof(float));
    if (element->value == NULL) {
        errno = ENOMEM;
        return 2;
    }
    *(float*)element->value = value;
    element->valuetype = FLT;
    return 0;
}

// Resets pairs value to the given string
int setrawH(obj_t *pair, char *val_ptr, int str_len) {
    if (pair == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(pair->value);
    pair->value = malloc(str_len + 1);
    if (pair->value == NULL) {
        errno = ENOMEM;
        return 2;
    }
    strncpy(pair->value, val_ptr, str_len);
    ((char*)pair->value)[str_len] = '\0';
    pair->valuetype = RAW;
    return 0;
}

int setrawL(array_t *element, char *val_ptr, unsigned int str_len) {
    if (element == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(element->value);
    element->value = malloc(str_len + 1);
    if (element->value == NULL) {
        errno = ENOMEM;
        return 2;
    }
    strncpy(element->value, val_ptr, str_len);
    ((char*)element->value)[str_len] = '\0';
    element->valuetype = RAW;
    return 0;
}

// Resets pairs value to the given double
int setdoubleH(obj_t *pair, double value) {
    if (pair == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(double));
    if (pair->value == NULL) {
        errno = ENOMEM;
        return 2;
    }
    *(double*)pair->value = value;
    pair->valuetype = DBL;
    return 0;
}

int setdoubleL(array_t *element, double value) {
    if (element == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(element->value);
    element->value = malloc(sizeof(value));
    if (element->value == NULL) {
        errno = ENOMEM;
        return 2;
    }
    *(double*)element->value = value;
    element->valuetype = DBL;
    return 0;
}

// Resets pairs value to the given long
int setlongH(obj_t *pair, long value) {
    if (pair == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(long));
    if (pair->value == NULL) {
        errno = ENOMEM;
        return 2;
    }
    *(long*)pair->value = value;
    pair->valuetype = LONG;
    return 0;
}

int setlongL(array_t *element, long value) {
    if (element == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(element->value);
    element->value = malloc(sizeof(long));
    if (element->value == NULL) {
        errno = ENOMEM;
        return 2;
    }
    *(long*)element->value = value;
    element->valuetype = LONG;
    return 0;
}

// Resets pairs value to the given long long
int setlonglongH(obj_t *pair, long long value) {
    if (pair == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(long long));
    if (pair->value == NULL) {
        errno = ENOMEM;
        return 2;
    }
    *(long long*)pair->value = value;
    pair->valuetype = LL;
    return 0;
}

int setlonglongL(array_t *element, long long value) {
    if (element == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(element->value);
    element->value = malloc(sizeof(long long));
    if (element->value == NULL) {
        errno = ENOMEM;
        return 2;
    }
    *(long long*)element->value = value;
    element->valuetype = LL;
    return 0;
}


// Resets pairs value to the given long double
int setlongdoubleH(obj_t *pair, long double value) {
    if (pair == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(pair->value);
    pair->value = malloc(sizeof(long double));
    if (pair->value == NULL) {
        errno = ENOMEM;
        return 2;
    }
    *(long double*)pair->value = value;
    pair->valuetype = LDBL;
    return 0;
}

int setlongdoubleL(array_t *element, long double value) {
    if (element == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(element->value);
    element->value = malloc(sizeof(long double));
    if (element->value == NULL) {
        errno = ENOMEM;
        return 2;
    }
    *(long double*)element->value = value;
    element->valuetype = LDBL;
    return 0;
}

// Resets pairs value to the given list
int setlistH(obj_t *pair, array_t *element) {
    if (pair == NULL || element == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(pair->value);
    pair->value = element;
    pair->valuetype = LIST;
    return 0;
}


int setlistL(array_t *element, array_t *e2) {
    if (element == NULL || e2 == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(element->value);
    element->value = e2;
    element->valuetype = LIST;
    return 0;
}

// Resets pairs value to the given hashmap
int setmapH(obj_t *pair, obj_t *p2) {
    if (pair == NULL || p2 == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(pair->value);
    pair->value = p2;
    pair->valuetype = NMAP;
    return 0;
}

int setmapL(array_t *element, obj_t *map) {
    if (element == NULL || map == NULL) {
        errno = EINVAL;
        return 1;
    }
    free(element->value);
    element->value = map;
    element->valuetype = NMAP;
    return 0;
}