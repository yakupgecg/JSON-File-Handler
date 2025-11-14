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

// Returns a string representation of vt
char *get_vt(enum valuetype vt) {
    switch (vt) {
        case STR: return "STR";
        case INT: return "INT";
        case DBL: return "DBL";
        case LIST: return "LIST";
        case OBJ: return "OBJ";
        default: return "NULL";
    }
}

// Returns the length of the map, but if the pair is not the root of the map, it will start from pair
size_t map_len(obj_t *map) {
    size_t len = 0;
    obj_t *current = map;
    while (current != NULL) {
        len += 1;
        current = current->next;
    }
    return len;
}

// Returns the length of the list
size_t list_len(array_t *list) {
    size_t len = 0;
    array_t *current = list;
    while (current != NULL) {
        len += 1;
        current = current->next;
    }
    return len;
}

//Frees a json value
int free_json_value(json_value_t *val) {
    if (val == NULL) {
        errno = EINVAL;
        return 1;
    }
    if (val->vt == OBJ) {
        if (val->value.obj != NULL) free_map(val->value.obj);
        val->value.obj = NULL;
    } else if (val->vt == LIST) {
        if (val->value.arr != NULL) free_list(val->value.arr);
        val->value.arr = NULL;
    } else if (val->vt == STR) {
        if (val->value.str.str != NULL) free(val->value.str.str);
        val->value.str.str = NULL;
        val->value.str.len = 0;
    }
    val->vt = 0;
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
    if (free_json_value(&pair->value) == 1) {
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
    if (free_json_value(&element->value) == 1) {
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
array_t *getelementbyindex(array_t *root, size_t index) {
    if (root == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (list_len(root) < index) return last_element(root);
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
    map->key = malloc(1);
    if (map->key == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    map->next = NULL;
    map->prev = NULL;
    map->value.value.str.str = NULL;
    map->value.vt = 0;
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
    list->prev = NULL;
    list->value.value.str.str = NULL;
    list->value.vt = 0;
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
    current->next->prev = current;
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
    current->next->prev = current;
    if (current->next == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    return current->next;
}

// Adds an object after the given obj
obj_t *insertH(obj_t *obj) {
    if (!obj) {
        errno = EINVAL;
        return NULL;
    }
    obj_t *newobj = initM();
    if (!newobj) {
        errno = ENOMEM;
        return NULL;
    }
    newobj->next = obj->next;
    newobj->prev = obj;
    if (obj->next != NULL) {
        obj->next->prev = newobj;
    }
    obj->next = newobj;
    return newobj;
}

//Adds an element after the given element
array_t *insertL(array_t *element) {
    if (!element) {
        errno = EINVAL;
        return NULL;
    }
    array_t *newelement = initL();
    if (!newelement) {
        errno = ENOMEM;
        return NULL;
    }
    newelement->next = element->next;
    newelement->prev = element;
    if (element->next != NULL) {
        element->next->prev = newelement;
    }
    element->next = newelement;
    return newelement;
}

// Removes the last object and returns it
obj_t *popH(obj_t *obj) {
    if (!obj) {
        errno = EINVAL;
        return NULL;
    }
    obj_t *last = last_pair(obj);
    last->prev->next = NULL;
    return last;
}

// Removes the last element and returns it
array_t *popL(array_t *element) {
    if (!element) {
        errno = EINVAL;
        return NULL;
    }
    array_t *last = last_element(element);
    last->prev->next = NULL;
    return last;
}

// Resets pairs key to the given string
obj_t *resetkey(obj_t *pair, char *key) {
    if (pair == NULL || key == NULL) {
        errno = EINVAL;
        return NULL;
    }
    int str_len = strlen(key);
    free(pair->key);
    pair->key = str_dup(key);
    if (pair->key == NULL) {
        return NULL;
    }
    pair->key[str_len] = '\0';
    return pair;
}

void setval(json_value_t *value, void *src, enum valuetype vt) {
    if (value == NULL) {
        errno = EINVAL;
        return;
    }

    free_json_value(value);

    switch (vt) {
        case STR: {
            value->value.str.str = str_dup((char*)src);
            value->value.str.len = strlen((char*)src);
            value->vt = STR;
            break;
        }
        case INT: value->value.i = *(int*)src; value->vt = INT; break;
        case DBL: value->value.dbl = *(double*)src; value->vt = DBL; break;
        case OBJ: value->value.obj = (obj_t*)src;  value->vt = OBJ; break;
        case LIST: value->value.arr = (array_t*)src; value->vt = LIST; break;
        default: errno = EINVAL; return;
    }
}

// Sets the object's or element's value to a string
obj_t *setstrH(obj_t *obj, char *key, char *src) {
    if (obj == NULL || src == NULL) {
        errno = EINVAL;
        return NULL;
    }
    
    if (key != NULL) {
        if (resetkey(obj, key) == NULL) {
            return NULL;
        }
    }
    setval(&obj->value, src, STR);
    return obj;
}

array_t *setstrL(array_t *arr, char *src) {
    if (arr == NULL || src == NULL) {
        errno = EINVAL;
        return NULL;
    }
    
    setval(&arr->value, src, STR);
    return arr;
}

// Sets the object's or element's value to an integer
obj_t *setintH(obj_t *obj, char *key, int src) {
    if (obj == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (key != NULL) {
        if (resetkey(obj, key) == NULL) {
            return NULL;
        }
    }
    setval(&obj->value, &src, INT);
    return obj;
}

array_t *setintL(array_t *arr, int src) {
    if (arr == NULL) {
        errno = EINVAL;
        return NULL;
    }
    
    setval(&arr->value, &src, INT);
    return arr;
}

// Sets the object's or element's to a double
obj_t *setdoubleH(obj_t *obj, char *key, double src) {
    if (obj == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (key != NULL) {
        if (resetkey(obj, key) == NULL) {
            return NULL;
        }
    }
    setval(&obj->value, &src, DBL);
    return obj;
}

array_t *setdoubleL(array_t *arr, double src) {
    if (arr == NULL) {
        errno = EINVAL;
        return NULL;
    }
    
    setval(&arr->value, &src, DBL);
    return arr;
}

// Sets the object's or element's to an object
obj_t *setobjH(obj_t *obj, char *key, obj_t *src) {
    if (obj == NULL || src == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (key != NULL) {
        if (resetkey(obj, key) == NULL) {
            return NULL;
        }
    }
    setval(&obj->value, src, OBJ);
    return obj;
}

array_t *setobjL(array_t *arr, obj_t *src) {
    if (arr == NULL || src == NULL) {
        errno = EINVAL;
        return NULL;
    }
    
    setval(&arr->value, src, OBJ);
    return arr;
}

// Sets the object's or element's to an array
obj_t *setarrH(obj_t *obj, char *key, array_t *src) {
    if (obj == NULL || src == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (key != NULL) {
        if (resetkey(obj, key) == NULL) {
            return NULL;
        }
    }
    setval(&obj->value, src, LIST);
    return obj;
}

array_t *setarrL(array_t *arr, array_t *src) {
    if (arr == NULL || src == NULL) {
        errno = EINVAL;
        return NULL;
    }
    
    setval(&arr->value, src, LIST);
    return arr;
}

// Copies the given object, either returns the copy or copies to another object.
obj_t *copy_obj(obj_t *obj, obj_t *cobj) {
    if (!obj) {
        errno = EINVAL;
        return NULL;
    }
    if (cobj != NULL) {
        switch (obj->value.vt) {
            case STR: setstrH(cobj, obj->key, obj->value.value.str.str); break;
            case INT: setintH(cobj, obj->key, obj->value.value.i); break;
            case DBL: setdoubleH(cobj, obj->key, obj->value.value.dbl); break;
            case OBJ: setobjH(cobj, obj->key, obj->value.value.obj); break;
            case LIST: setarrH(cobj, obj->key, obj->value.value.arr); break;
        }
        return cobj;
    }
    obj_t *newobj = initM();
    if (!newobj) {
        errno = ENOMEM;
        return NULL;
    }
    switch (obj->value.vt) {
        case STR: setstrH(newobj, obj->key, obj->value.value.str.str); break;
        case INT: setintH(newobj, obj->key, obj->value.value.i); break;
        case DBL: setdoubleH(newobj, obj->key, obj->value.value.dbl); break;
        case OBJ: setobjH(newobj, obj->key, obj->value.value.obj); break;
        case LIST: setarrH(newobj, obj->key, obj->value.value.arr); break;
    }
    return newobj;
}

// Copies the given element, either returns the copy or copies to another element.
array_t *copy_element(array_t *element, array_t *celement) {
    if (!element) {
        errno = EINVAL;
        return NULL;
    }
    if (celement != NULL) {
        switch (element->value.vt) {
            case STR: setstrL(celement, element->value.value.str.str); break;
            case INT: setintL(celement, element->value.value.i); break;
            case DBL: setdoubleL(celement, element->value.value.dbl); break;
            case OBJ: setobjL(celement, element->value.value.obj); break;
            case LIST: setarrL(celement, element->value.value.arr); break;
        }
        return celement;
    }
    array_t *newelement = initL();
    if (!newelement) {
        errno = ENOMEM;
        return NULL;
    }
    switch (element->value.vt) {
        case STR: setstrL(newelement, element->value.value.str.str); break;
        case INT: setintL(newelement, element->value.value.i); break;
        case DBL: setdoubleL(newelement, element->value.value.dbl); break;
        case OBJ: setobjL(newelement, element->value.value.obj); break;
        case LIST: setarrL(newelement, element->value.value.arr); break;
    }
    return newelement;
}