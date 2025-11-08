#include "..\Headers\utils.h"

// This is a helper function since I am on windows and cannot use normal stdup()
static char *str_dup(char *str) {
    if (str == NULL) {
        errno = EINVAL;
        return NULL;
    }
    int keylen = strlen(str) + 1;
    char *newstr = malloc(keylen * sizeof(char));
    if (newstr == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    memcpy(newstr, str, keylen);
    return newstr;
}

char *get_vt(enum valuetype vt) {
    switch (vt) {
        case RAW: return "RAW";
        case SHR: return "SHR";
        case INT: return "INT";
        case FLT: return "FLT";
        case DBL: return "DBL";
        case LONG: return "LONG";
        case LL: return "LL";
        case LDBL: return "LDBL";
        case LIST: return "LIST";
        case OBJ: return "OBJ";
        default: return "NULL";
    }
}

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
            case OBJ: bsize += map_size(current->value.value.obj); break;
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
            case OBJ: bsize += map_size(current->value.value.obj); break;
            default: errno = EINVAL; return -2;
        }
        current = current->next;
    }
    return bsize;
}

//Frees a json value
int free_json_value(json_value_t val) {
    if (val.vt == OBJ && val.value.obj != NULL) {
        free_map(val.value.obj);
    } else if (val.vt == LIST && val.value.arr != NULL) {
        free_list(val.value.arr);
    } else if (val.vt == RAW && val.value.str.str != NULL) {
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

void setvalH(obj_t *obj, void *src, enum valuetype vt) {
    if (obj == NULL) {
        errno = EINVAL;
        return;
    }

    free_json_value(obj->value);

    switch (vt) {
        case RAW: {
            obj->value.value.str.str = str_dup((char*)src);
            obj->value.value.str.len = strlen((char*)src);
            obj->value.vt = RAW;
            break;
        }
        case INT: obj->value.value.i = *(int*)src; obj->value.vt = INT; break;
        case SHR: obj->value.value.s = *(short*)src; obj->value.vt = SHR; break;
        case FLT: obj->value.value.f = *(float*)src; obj->value.vt = FLT; break;
        case DBL: obj->value.value.dbl = *(double*)src; obj->value.vt = DBL; break;
        case LONG: obj->value.value.lg = *(long*)src; obj->value.vt = LONG; break;
        case LL: obj->value.value.lgl = *(long long*)src; obj->value.vt = LL; break;
        case LDBL: obj->value.value.ldbl = *(long double*)src; obj->value.vt = LDBL; break;
        case OBJ: obj->value.value.obj = (obj_t*)src;  obj->value.vt = OBJ; break;
        case LIST: obj->value.value.arr = (array_t*)src; obj->value.vt = LIST; break;
        default: errno = EINVAL; return;
    }
}
void setvalL(array_t *arr, void *src, enum valuetype vt) {
    if (arr == NULL) {
        errno = EINVAL;
        return;
    }

    free_json_value(arr->value);

    switch (vt) {
        case RAW: {
            arr->value.value.str.str = str_dup((char*)src);
            arr->value.value.str.len = strlen((char*)src);
            arr->value.vt = RAW;
            break;
        }
        case INT: arr->value.value.i = *(int*)src; arr->value.vt = INT; break;
        case SHR: arr->value.value.s = *(short*)src; arr->value.vt = SHR; break;
        case FLT: arr->value.value.f = *(float*)src; arr->value.vt = FLT; break;
        case DBL: arr->value.value.dbl = *(double*)src; arr->value.vt = DBL; break;
        case LONG: arr->value.value.lg = *(long*)src; arr->value.vt = LONG; break;
        case LL: arr->value.value.lgl = *(long long*)src; arr->value.vt = LL; break;
        case LDBL: arr->value.value.ldbl = *(long double*)src; arr->value.vt = LDBL; break;
        case OBJ: arr->value.value.obj = (obj_t*)src;  arr->value.vt = OBJ; break;
        case LIST: arr->value.value.arr = (array_t*)src; arr->value.vt = LIST; break;
        default: errno = EINVAL; return;
    }
}