#include "..\Headers\utils.h"

// Returns the length of the map, but if the pair is not the root of the map, it will start from pair
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
        switch (current->value.vt) {
            case RAW: bsize += current->value.value.str.len; break;
            case SHR: bsize += SHR_STR_LEN; break;
            case INT: bsize += INT_STR_LEN; break;
            case FLT: bsize += FLT_STR_LEN; break;
            case DBL: bsize += DBL_STR_LEN; break;
            case LONG: bsize += LONG_STR_LEN; break;
            case LL: bsize += LL_STR_LEN; break;
            case LDBL: bsize += LDBL_STR_LEN; break;
            case LIST: bsize += list_size(current->value.value.arr); break;
            case NMAP: bsize += map_size(current->value.value.obj); break;
            default: errno = EINVAL; return -2;
        }
        current = current->next;
    }
    return bsize;
}

// Returns the size of the map
int map_size(obj_t *root) {
    if (root == NULL) {
        errno = EINVAL;
        return -1;
    }
    int bsize = 0;
    obj_t *current = root;
    while (current != NULL) {
        bsize += strlen(current->key) + 2;
        switch (current->value.vt) {
            case RAW: bsize += current->value.value.str.len; break;
            case SHR: bsize += SHR_STR_LEN; break;
            case INT: bsize += INT_STR_LEN; break;
            case FLT: bsize += FLT_STR_LEN; break;
            case DBL: bsize += DBL_STR_LEN; break;
            case LONG: bsize += LONG_STR_LEN; break;
            case LL: bsize += LL_STR_LEN; break;
            case LDBL: bsize += LDBL_STR_LEN; break;
            case LIST: bsize += list_size(current->value.value.arr); break;
            case NMAP: bsize += map_size(current->value.value.obj); break;
            default: errno = EINVAL; return -2;
        }
        current = current->next;
    }
    return bsize;
}

//Frees a json value
int free_json_value(json_value_t val) {
    if (val.vt == NMAP) {
        free_map(val.value.obj);
    } else if (val.vt == LIST) {
        free_list(val.value.arr);
    } else if (val.vt == RAW) {
        free(val.value.str.str);
    }
    return 0;
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
    if (free_json_value(pair->value) == 1) {
        free(pair);
        errno = EINVAL;
        return 1;
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
    if (free_json_value(element->value) == 1) {
        free(element);
        errno = EINVAL;
        return 1;
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

// Returns the map, which has the key to find
obj_t *getpairbykey(obj_t *root, char *key) {
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

// This will initalize a map and then return a pointer to it
obj_t *initM() {
    obj_t *map = malloc(sizeof(obj_t));
    if (map == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    map->key = malloc(sizeof(char));
    if (map->key == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    map->next = NULL;
    return map;
}

// Returns the last pair in a map
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

// Resets pairs key to the given string
obj_t *resetkey(obj_t *pair, char *key) {
    if (pair == NULL || key == NULL) {
        errno = EINVAL;
        return NULL;
    }
    int str_len = strlen(key);
    free(pair->key);
    pair->key = malloc(str_len + 1);
    if (pair->key == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    memcpy(pair->key, key, str_len);
    pair->key[str_len] = '\0';
    return pair;
}

// Resets pairs value to the given integer
int setintH(obj_t *pair, int value) {
    if (pair == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (pair->value.vt == NMAP) free_map(pair->value.value.obj);
    else if (pair->value.vt == LIST) free_list(pair->value.value.arr);
    else if (pair->value.vt == RAW) free(pair->value.value.str.str);
    pair->value.value.i = value;
    pair->value.vt = INT;
    return 0;
}

int setintL(array_t *element, int value) {
    if (element == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (element->value.vt == NMAP) free_map(element->value.value.obj);
    else if (element->value.vt == LIST) free_list(element->value.value.arr);
    else if (element->value.vt == RAW) free(element->value.value.str.str);
    element->value.value.i = value;
    element->value.vt = INT;
    return 0;
}

// Resets pairs value to the given short
int setshortH(obj_t *pair, short value) {
    if (pair == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (pair->value.vt == NMAP) free_map(pair->value.value.obj);
    else if (pair->value.vt == LIST) free_list(pair->value.value.arr);
    else if (pair->value.vt == RAW) free(pair->value.value.str.str);
    pair->value.value.s = value;
    pair->value.vt = SHR;
    return 0;
}

int setshortL(array_t *element, short value) {
    if (element == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (element->value.vt == NMAP) free_map(element->value.value.obj);
    else if (element->value.vt == LIST) free_list(element->value.value.arr);
    else if (element->value.vt == RAW) free(element->value.value.str.str);
    element->value.value.s = value;
    element->value.vt = SHR;
    return 0;
}

// Resets pairs value to the given float
int setfloatH(obj_t *pair, float value) {
    if (pair == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (pair->value.vt == NMAP) free_map(pair->value.value.obj);
    else if (pair->value.vt == LIST) free_list(pair->value.value.arr);
    else if (pair->value.vt == RAW) free(pair->value.value.str.str);
    pair->value.value.f = value;
    pair->value.vt = FLT;
    return 0;
}

int setfloatL(array_t *element, float value) {
    if (element == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (element->value.vt == NMAP) free_map(element->value.value.obj);
    else if (element->value.vt == LIST) free_list(element->value.value.arr);
    else if (element->value.vt == RAW) free(element->value.value.str.str);
    element->value.value.f = value;
    element->value.vt = FLT;
    return 0;
}

// Resets pairs value to the given string
int setrawH(obj_t *pair, char *val_ptr) {
    if (pair == NULL) {
        errno = EINVAL;
        return 1;
    }
    int str_len = strlen(val_ptr) + 1;
    if (pair->value.vt == NMAP) free_map(pair->value.value.obj);
    else if (pair->value.vt == LIST) free_list(pair->value.value.arr);
    else if (pair->value.vt == RAW) free(pair->value.value.str.str);
    pair->value.value.str.str = malloc(str_len + 1);
    pair->value.value.str.len = str_len;
    memcpy(pair->value.value.str.str, val_ptr, str_len + 1);
    pair->value.vt = RAW;
    return 0;
}

int setrawL(array_t *element, char *val_ptr) {
    if (element == NULL) {
        errno = EINVAL;
        return 1;
    }
    int str_len = strlen(val_ptr);
    if (element->value.vt == NMAP) free_map(element->value.value.obj);
    else if (element->value.vt == LIST) free_list(element->value.value.arr);
    else if (element->value.vt == RAW) free(element->value.value.str.str);
    element->value.value.str.str = malloc(str_len + 1);
    element->value.value.str.len = str_len;
    memcpy(element->value.value.str.str, val_ptr, str_len + 1);
    element->value.vt = RAW;
    return 0;
}

// Resets pairs value to the given double
int setdoubleH(obj_t *pair, double value) {
    if (pair == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (pair->value.vt == NMAP) free_map(pair->value.value.obj);
    else if (pair->value.vt == LIST) free_list(pair->value.value.arr);
    else if (pair->value.vt == RAW) free(pair->value.value.str.str);
    pair->value.value.dbl = value;
    pair->value.vt = DBL;
    return 0;
}

int setdoubleL(array_t *element, double value) {
    if (element == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (element->value.vt == NMAP) free_map(element->value.value.obj);
    else if (element->value.vt == LIST) free_list(element->value.value.arr);
    else if (element->value.vt == RAW) free(element->value.value.str.str);
    element->value.value.dbl = value;
    element->value.vt = DBL;
    return 0;
}

// Resets pairs value to the given long
int setlongH(obj_t *pair, long value) {
    if (pair == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (pair->value.vt == NMAP) free_map(pair->value.value.obj);
    else if (pair->value.vt == LIST) free_list(pair->value.value.arr);
    else if (pair->value.vt == RAW) free(pair->value.value.str.str);
    pair->value.value.lg = value;
    pair->value.vt = LONG;
    return 0;
}

int setlongL(array_t *element, long value) {
    if (element == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (element->value.vt == NMAP) free_map(element->value.value.obj);
    else if (element->value.vt == LIST) free_list(element->value.value.arr);
    else if (element->value.vt == RAW) free(element->value.value.str.str);
    element->value.value.lg = value;
    element->value.vt = LONG;
    return 0;
}

// Resets pairs value to the given long long
int setlonglongH(obj_t *pair, long long value) {
    if (pair == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (pair->value.vt == NMAP) free_map(pair->value.value.obj);
    else if (pair->value.vt == LIST) free_list(pair->value.value.arr);
    else if (pair->value.vt == RAW) free(pair->value.value.str.str);
    pair->value.value.lgl = value;
    pair->value.vt = LL;
    return 0;
}

int setlonglongL(array_t *element, long long value) {
    if (element == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (element->value.vt == NMAP) free_map(element->value.value.obj);
    else if (element->value.vt == LIST) free_list(element->value.value.arr);
    else if (element->value.vt == RAW) free(element->value.value.str.str);
    element->value.value.lgl = value;
    element->value.vt = LL;
    return 0;
}


// Resets pairs value to the given long double
int setlongdoubleH(obj_t *pair, long double value) {
    if (pair == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (pair->value.vt == NMAP) free_map(pair->value.value.obj);
    else if (pair->value.vt == LIST) free_list(pair->value.value.arr);
    else if (pair->value.vt == RAW) free(pair->value.value.str.str);
    pair->value.value.ldbl = value;
    pair->value.vt = LDBL;
    return 0;
}

int setlongdoubleL(array_t *element, long double value) {
    if (element == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (element->value.vt == NMAP) free_map(element->value.value.obj);
    else if (element->value.vt == LIST) free_list(element->value.value.arr);
    else if (element->value.vt == RAW) free(element->value.value.str.str);
    element->value.value.lgl = value;
    element->value.vt = LL;
    return 0;
}

// Resets pairs value to the given list
int setlistH(obj_t *pair, array_t *element) {
    if (pair == NULL || element == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (pair->value.vt == NMAP) free_map(pair->value.value.obj);
    else if (pair->value.vt == LIST) free_list(pair->value.value.arr);
    else if (pair->value.vt == RAW) free(pair->value.value.str.str);
    pair->value.value.arr = element;
    pair->value.vt = LIST;
    return 0;
}


int setlistL(array_t *element, array_t *e2) {
    if (element == NULL || e2 == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (element->value.vt == NMAP) free_map(element->value.value.obj);
    else if (element->value.vt == LIST) free_list(element->value.value.arr);
    else if (element->value.vt == RAW) free(element->value.value.str.str);
    element->value.value.arr = e2;
    element->value.vt = LIST;
    return 0;
}

// Resets pairs value to the given map
int setmapH(obj_t *pair, obj_t *p2) {
    if (pair == NULL || p2 == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (pair->value.vt == NMAP) free_map(pair->value.value.obj);
    else if (pair->value.vt == LIST) free_list(pair->value.value.arr);
    else if (pair->value.vt == RAW) free(pair->value.value.str.str);
    pair->value.value.obj = p2;
    pair->value.vt = NMAP;
    return 0;
}

int setmapL(array_t *element, obj_t *pair) {
    if (element == NULL || pair == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (element->value.vt == NMAP) free_map(element->value.value.obj);
    else if (element->value.vt == LIST) free_list(element->value.value.arr);
    else if (element->value.vt == RAW) free(element->value.value.str.str);
    element->value.value.obj = pair;
    element->value.vt = NMAP;
    return 0;
}