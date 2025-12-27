#include "..\Headers\utils.h"

// This is a helper function since I am on windows and cannot use normal stdup()
static char *str_dup(char *str) {
    if (!str) {
        errno = EINVAL;
        return NULL;
    }
    int len = strlen(str) + 1;
    char *newstr = malloc(len * sizeof(char));
    if (!newstr) {
        errno = ENOMEM;
        return NULL;
    }
    memcpy(newstr, str, len);
    return newstr;
}

static char *quots(char *str) {
    if (!str) {
        errno = EINVAL;
        return NULL;
    }
    int len = strlen(str) + 3;
    char *newstr = malloc(len);
    if (!newstr) {
        errno = ENOMEM;
        return NULL;
    }
    char *cur = str;
    char *newcur = newstr;
    *newcur = '\"';
    newcur++;
    while (*cur) {
        *newcur = *cur;
        cur++;
        newcur++;
    }
    *newcur = '\"';
    newcur++;
    *newcur = '\0';
    return newstr;
}

// Returns a string representation of vt
char *JFH_get_vt(enum jfh_valuetype vt) {
    switch (vt) {
        case JFH_STR: return "STR";
        case JFH_INT: return "INT";
        case JFH_DBL: return "DBL";
        case JFH_OBJ: return "OBJ";
        case JFH_LIST: return "LIST";
        case JFH_BOOL: return "BOOL";
        case JFH_NULL: return "NULL";
        default: return "(unknown type)";
    }
}

// Returns the length of the map, but if the pair is not the root of the map, it will start from pair
size_t JFH_map_len(jfh_obj_t *map) {
    if (!map) {
        errno = EINVAL;
        return 0;
    }
    size_t len = 0;
    jfh_obj_t *current = map;
    while (current) {
        len += 1;
        current = current->next;
    }
    return len;
}

// Returns the length of the list
size_t JFH_list_len(jfh_array_t *list) {
    if (!list) {
        errno = EINVAL;
        return 0;
    }
    size_t len = 0;
    jfh_array_t *current = list;
    while (current) {
        len += 1;
        current = current->next;
    }
    return len;
}

//Frees a json value
int JFH_free_json_value(jfh_json_value_t *val) {
    if (!val) {
        errno = EINVAL;
        return 1;
    }
    if (val->vt == JFH_OBJ) {
        if (val->value.obj) JFH_free_map(val->value.obj);
        val->value.obj = NULL;
    } else if (val->vt == JFH_LIST) {
        if (val->value.arr) JFH_free_list(val->value.arr);
        val->value.arr = NULL;
    } else if (val->vt == JFH_STR) {
        if (val->value.str.str) free(val->value.str.str);
        val->value.str.str = NULL;
        val->value.str.len = 0;
    }
    val->vt = 0;
    return 0;
}

// Forcefully frees a pair
int JFH_free_pair(jfh_obj_t *pair) {
    if (!pair) {
        errno = EINVAL;
        return 1;
    }
    if (pair->key) {
        free(pair->key);
    }
    if (JFH_free_json_value(&pair->value) == 1) {
        free(pair);
        errno = EINVAL;
        return 1;
    }
    free(pair);
    return 0;
}

// Forcefully frees an element
int JFH_free_element(jfh_array_t *element) {
    if (!element) {
        errno = EINVAL;
        return 1;
    }
    if (!JFH_free_json_value(&element->value)) {
        free(element);
        errno = EINVAL;
        return 1;
    }
    free(element);
    return 0;
}

// Frees all the pairs after map and itself
int JFH_free_map(jfh_obj_t* map) {
    if (!map) {
        errno = EINVAL;
        return 1;
    }
    jfh_obj_t *current = map;
    jfh_obj_t *next;
    while (current) {
        next = current->next;
        JFH_free_pair(current);
        current = next;
    }
    return 0;
}

// Frees the list (or frees every element after the given element)
int JFH_free_list(jfh_array_t *list) {
    if (!list) {
        errno = EINVAL;
        return 1;
    }
    jfh_array_t *current = list;
    jfh_array_t *next;
    while (current) {
        next = current->next;
        JFH_free_element(current);
        current = next;
    }
    return 0;
}

// Returns the map, which has the key to find
jfh_obj_t *JFH_pairbykey(jfh_obj_t *root, char *key) {
    if (!root || !key) {
        errno = EINVAL;
        return NULL;
    }
    jfh_obj_t *current = root;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Returns the first object found anywhere in the root.
jfh_obj_t *JFH_searchH(jfh_obj_t *root, char *key) {
    if (!root || !key) {
        errno = EINVAL;
        return NULL;
    }
    jfh_obj_t *current = root;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current;
        }
        if (current->value.vt == JFH_OBJ) {
            jfh_obj_t *result = JFH_searchH(current->value.value.obj, key);
            if (result) {
                return result;
            }
        } else if (current->value.vt == JFH_LIST) {
            jfh_obj_t *result = JFH_searchL(current->value.value.arr, key);
            if (result) {
                return result;
            }
        }
        current = current->next;
    }
    return NULL;
}

// Returns the first object found anywhere in the list.
jfh_obj_t *JFH_searchL(jfh_array_t *root, char *key) {
    if (!root || !key) {
        errno = EINVAL;
        return NULL;
    }
    jfh_array_t *current = root;
    while (current) {
        if (current->value.vt == JFH_OBJ) {
            jfh_obj_t *result = JFH_searchH(current->value.value.obj, key);
            if (result) {
                return result;
            }
        } else if (current->value.vt == JFH_LIST) {
            jfh_obj_t *result = JFH_searchL(current->value.value.arr, key);
            if (result) {
                return result;
            }
        }
        current = current->next;
    }
    return NULL;
}

// Returns the element by index. For example if index is 1 it returns root->next
jfh_array_t *JFH_getelementbyindex(jfh_array_t *root, size_t index) {
    if (!root) {
        errno = EINVAL;
        return NULL;
    }
    if (JFH_list_len(root) < index) return JFH_last_element(root);
    jfh_array_t *current = root;
    size_t i;
    for (i = 0; i < index; i++) {
        current = current->next;
    }
    return current;
}

// This will initalize a map and then return a pointer to it
jfh_obj_t *JFH_initM() {
    jfh_obj_t *map = malloc(sizeof(jfh_obj_t));
    if (!map) {
        errno = ENOMEM;
        return NULL;
    }
    map->key = malloc(1);
    if (!map->key) {
        errno = ENOMEM;
        free(map);
        return NULL;
    }
    map->next = NULL;
    map->prev = NULL;
    map->value.value.str.str = NULL;
    map->value.vt = 0;
    map->empty = true;
    return map;
}

// Returns the last pair in a map
jfh_obj_t *JFH_last_pair(jfh_obj_t *root) {
    if (!root) {
        errno = EINVAL;
        return NULL;
    }
    jfh_obj_t *current = root;
    while (current->next) {
        current = current->next;
    }
    return current;
}

// Returns the last element in a list
jfh_array_t *JFH_last_element(jfh_array_t *root) {
    if (!root) {
        errno = EINVAL;
        return NULL;
    }
    jfh_array_t *current = root;
    while (current->next) {
        current = current->next;
    }
    return current;
}

// Initializes a list
jfh_array_t *JFH_initL() {
    jfh_array_t *list = malloc(sizeof(jfh_array_t));
    if (!list) {
        errno = ENOMEM;
        return NULL;
    }
    list->next = NULL;
    list->prev = NULL;
    list->value.value.str.str = NULL;
    list->value.vt = 0;
    list->empty = true;
    return list;
}

// Adds a pair to the end of the given map and returns it
jfh_obj_t *JFH_appendH(jfh_obj_t *map) {
    if (!map) {
        errno = EINVAL;
        return NULL;
    }
    map->empty = false;
    jfh_obj_t *current = map;
    while (current->next) {
        current = current->next;
    }
    current->next = JFH_initM();
    if (!current->next) {
        return NULL;
    }
    current->next->prev = current;
    return current->next;
}

// Adds an element to the end of the given list and returns it
jfh_array_t *JFH_appendL(jfh_array_t *list) {
    if (!list) {
        errno = EINVAL;
        return NULL;
    }
    list->empty = false;
    jfh_array_t *current = list;
    while (current->next) {
        current = current->next;
    }
    current->next = JFH_initL();
    if (!current->next) {
        return NULL;
    }
    current->next->prev = current;
    return current->next;
}

// Adds an object after the given obj
jfh_obj_t *JFH_insertH(jfh_obj_t *obj) {
    if (!obj) {
        errno = EINVAL;
        return NULL;
    }
    obj->empty = false;
    jfh_obj_t *newobj = JFH_initM();
    if (!newobj) {
        errno = ENOMEM;
        return NULL;
    }
    newobj->next = obj->next;
    newobj->prev = obj;
    if (obj->next) {
        obj->next->prev = newobj;
    }
    obj->next = newobj;
    return newobj;
}

//Adds an element after the given element
jfh_array_t *JFH_insertL(jfh_array_t *element) {
    if (!element) {
        errno = EINVAL;
        return NULL;
    }
    element->empty = false;
    jfh_array_t *newelement = JFH_initL();
    if (!newelement) {
        errno = ENOMEM;
        return NULL;
    }
    newelement->next = element->next;
    newelement->prev = element;
    if (element->next) {
        element->next->prev = newelement;
    }
    element->next = newelement;
    return newelement;
}

// Removes the last object and returns it
jfh_obj_t *JFH_popH(jfh_obj_t *obj) {
    if (!obj) {
        errno = EINVAL;
        return NULL;
    }
    jfh_obj_t *last = JFH_last_pair(obj);
    last->prev->next = NULL;
    last->prev = NULL;
    return last;
}

// Removes the last element and returns it
jfh_array_t *JFH_popL(jfh_array_t *element) {
    if (!element) {
        errno = EINVAL;
        return NULL;
    }
    jfh_array_t *last = JFH_last_element(element);
    last->prev->next = NULL;
    last->prev = NULL;
    return last;
}

// Resets pairs key to the given string
jfh_obj_t *JFH_resetkey(jfh_obj_t *pair, char *key) {
    if (!pair || !key) {
        errno = EINVAL;
        return NULL;
    }
    pair->empty = false;
    int str_len = strlen(key);
    free(pair->key);
    pair->key = str_dup(key);
    if (!pair->key) {
        return NULL;
    }
    pair->key[str_len] = '\0';
    return pair;
}

void JFH_setval(jfh_json_value_t *value, void *src, enum jfh_valuetype vt) {
    if (!value || !src) {
        errno = EINVAL;
        return;
    }
    
    JFH_free_json_value(value);

    switch (vt) {
        case JFH_STR: {
            value->value.str.str = quots((char*)src);
            if (!value->value.str.str) return;
            value->value.str.len = strlen((char*)src);
            value->vt = JFH_STR;
            break;
        }
        case JFH_INT: value->value.i = *(int*)src; value->vt = JFH_INT; break;
        case JFH_DBL: value->value.dbl = *(double*)src; value->vt = JFH_DBL; break;
        case JFH_OBJ: value->value.obj = (jfh_obj_t*)src;  value->vt = JFH_OBJ; break;
        case JFH_LIST: value->value.arr = (jfh_array_t*)src; value->vt = JFH_LIST; break;
        case JFH_BOOL: value->value.b = *(bool*)src; value->vt = JFH_BOOL; break;
        case JFH_NULL: {
            value->value.str.str = str_dup((char*)src);
            if (!value->value.str.str) return;
            value->value.str.len = strlen((char*)src);
            value->vt = JFH_NULL;
            break;
        }
        default: errno = EINVAL; return;
    }
}

// Sets the object's or element's value to a string
jfh_obj_t *JFH_setstrH(jfh_obj_t *obj, char *key, char *src) {
    if (!obj || !key || !src) {
        errno = EINVAL;
        return NULL;
    }
    obj->empty = false;
    if (key) {
        if (!JFH_resetkey(obj, key)) {
            return NULL;
        }
    }
    JFH_setval(&obj->value, src, JFH_STR);
    return obj;
}

jfh_array_t *JFH_setstrL(jfh_array_t *arr, char *src) {
    if (!arr || !src) {
        errno = EINVAL;
        return NULL;
    }
    arr->empty = false;
    JFH_setval(&arr->value, src, JFH_STR);
    return arr;
}

// Sets the object's or element's value to an integer
jfh_obj_t *JFH_setintH(jfh_obj_t *obj, char *key, int src) {
    if (!obj || !key) {
        errno = EINVAL;
        return NULL;
    }
    obj->empty = false;
    if (key) {
        if (!JFH_resetkey(obj, key)) {
            return NULL;
        }
    }
    JFH_setval(&obj->value, &src, JFH_INT);
    return obj;
}

jfh_array_t *JFH_setintL(jfh_array_t *arr, int src) {
    if (!arr) {
        errno = EINVAL;
        return NULL;
    }
    arr->empty = false;
    JFH_setval(&arr->value, &src, JFH_INT);
    return arr;
}

// Sets the object's or element's to a double
jfh_obj_t *JFH_setdoubleH(jfh_obj_t *obj, char *key, double src) {
    if (!obj || !key) {
        errno = EINVAL;
        return NULL;
    }
    obj->empty = false;
    if (key) {
        if (!JFH_resetkey(obj, key)) {
            return NULL;
        }
    }
    JFH_setval(&obj->value, &src, JFH_DBL);
    return obj;
}

jfh_array_t *JFH_setdoubleL(jfh_array_t *arr, double src) {
    if (!arr) {
        errno = EINVAL;
        return NULL;
    }
    arr->empty = false;
    JFH_setval(&arr->value, &src, JFH_DBL);
    return arr;
}

// Sets the object's or element's to a boolean value
jfh_obj_t *JFH_setboolH(jfh_obj_t *obj, char *key, bool b) {
    if (!obj || !key) {
        errno = EINVAL;
        return NULL;
    }
    obj->empty = false;
    if (key) {
        if (!JFH_resetkey(obj, key)) {
            return NULL;
        }
    }
    JFH_setval(&obj->value, &b, JFH_BOOL);
    return obj;
}

jfh_array_t *JFH_setboolL(jfh_array_t *arr, bool b) {
    if (!arr) {
        errno = EINVAL;
        return NULL;
    }
    arr->empty = false;
    JFH_setval(&arr->value, &b, JFH_BOOL);
    return arr;
}

// Sets the object's or element's to null
jfh_obj_t *JFH_setnullH(jfh_obj_t *obj, char *key) {
    if (!obj || !key) {
        errno = EINVAL;
        return NULL;
    }
    obj->empty = false;
    if (key) {
        if (!JFH_resetkey(obj, key)) {
            return NULL;
        }
    }
    JFH_setval(&obj->value, "null", JFH_NULL);
    return obj;
}

jfh_array_t *JFH_setnullL(jfh_array_t *arr) {
    if (!arr) {
        errno = EINVAL;
        return NULL;
    }
    arr->empty = false;
    JFH_setval(&arr->value, "null", JFH_NULL);
    return arr;
}

// Sets the object's or element's value to a string with no quotes
jfh_obj_t *JFH_setstrH_nquots(jfh_obj_t *obj, char *key, char *src) {
    if (!obj || !key || !src) {
        errno = EINVAL;
        return NULL;
    }
    obj->empty = false;
    if (key) {
        if (!JFH_resetkey(obj, key)) {
            return NULL;
        }
    }
    JFH_setval(&obj->value, src, JFH_NULL);
    obj->value.vt = JFH_STR;
    return obj;
}

jfh_array_t *JFH_setstrL_nquots(jfh_array_t *arr, char *src) {
    if (!arr || !src) {
        errno = EINVAL;
        return NULL;
    }
    arr->empty = false;
    JFH_setval(&arr->value, src, JFH_NULL);
    arr->value.vt = JFH_STR;
    return arr;
}

// Sets the object's or element's to an object
jfh_obj_t *JFH_setobjH(jfh_obj_t *obj, char *key, jfh_obj_t *src) {
    if (!obj || !key || !src) {
        errno = EINVAL;
        return NULL;
    }
    obj->empty = false;
    if (key) {
        if (!JFH_resetkey(obj, key)) {
            return NULL;
        }
    }
    JFH_setval(&obj->value, src, JFH_OBJ);
    return obj;
}

jfh_array_t *JFH_setobjL(jfh_array_t *arr, jfh_obj_t *src) {
    if (!arr || !src) {
        errno = EINVAL;
        return NULL;
    }
    arr->empty = false;
    JFH_setval(&arr->value, src, JFH_OBJ);
    return arr;
}

// Sets the object's or element's to an array
jfh_obj_t *JFH_setarrH(jfh_obj_t *obj, char *key, jfh_array_t *src) {
    if (!obj || !key || !src) {
        errno = EINVAL;
        return NULL;
    }
    obj->empty = false;
    if (key) {
        if (!JFH_resetkey(obj, key)) {
            return NULL;
        }
    }
    JFH_setval(&obj->value, src, JFH_LIST);
    return obj;
}

jfh_array_t *JFH_setarrL(jfh_array_t *arr, jfh_array_t *src) {
    if (!arr || !src) {
        errno = EINVAL;
        return NULL;
    }
    
    JFH_setval(&arr->value, src, JFH_LIST);
    return arr;
}

// Copies the given object, either returns the copy or copies to another object.
jfh_obj_t *JFH_copy_obj(jfh_obj_t *obj, jfh_obj_t *cobj) {
    if (!obj || !cobj) {
        errno = EINVAL;
        return NULL;
    }
    if (cobj) {
        switch (obj->value.vt) {
            case JFH_STR: JFH_setstrH_nquots(cobj, obj->key, obj->value.value.str.str); break;
            case JFH_INT: JFH_setintH(cobj, obj->key, obj->value.value.i); break;
            case JFH_DBL: JFH_setdoubleH(cobj, obj->key, obj->value.value.dbl); break;
            case JFH_OBJ: JFH_setobjH(cobj, obj->key, JFH_copy_map(obj->value.value.obj, NULL)); break;
            case JFH_LIST: JFH_setarrH(cobj, obj->key, JFH_copy_list(obj->value.value.arr, NULL)); break;
            case JFH_BOOL: JFH_setboolH(cobj, obj->key, obj->value.value.b); break;
            case JFH_NULL: JFH_setnullH(cobj, obj->key); break;
            default: errno = EINVAL; return NULL;
        }
        return cobj;
    }
    jfh_obj_t *newobj = JFH_initM();
    if (!newobj) {
        errno = ENOMEM;
        return NULL;
    }
    switch (obj->value.vt) {
        case JFH_STR: JFH_setstrH_nquots(newobj, obj->key, obj->value.value.str.str); break;
        case JFH_INT: JFH_setintH(newobj, obj->key, obj->value.value.i); break;
        case JFH_DBL: JFH_setdoubleH(newobj, obj->key, obj->value.value.dbl); break;
        case JFH_OBJ: JFH_setobjH(newobj, obj->key, JFH_copy_map(obj->value.value.obj, NULL)); break;
        case JFH_LIST: JFH_setarrH(newobj, obj->key, JFH_copy_list(obj->value.value.arr, NULL)); break;
        case JFH_BOOL: JFH_setboolH(newobj, obj->key, obj->value.value.b); break;
        case JFH_NULL: JFH_setnullH(newobj, obj->key); break;
        default: errno = EINVAL; return NULL;
    }
    return newobj;
}

// Copies the given element, either returns the copy or copies to another element.
jfh_array_t *JFH_copy_element(jfh_array_t *element, jfh_array_t *celement) {
    if (!element || !celement) {
        errno = EINVAL;
        return NULL;
    }
    if (celement) {
        switch (element->value.vt) {
            case JFH_STR: JFH_setstrL_nquots(celement, element->value.value.str.str); break;
            case JFH_INT: JFH_setintL(celement, element->value.value.i); break;
            case JFH_DBL: JFH_setdoubleL(celement, element->value.value.dbl); break;
            case JFH_OBJ: JFH_setobjL(celement, JFH_copy_map(element->value.value.obj, NULL)); break;
            case JFH_LIST: JFH_setarrL(celement, JFH_copy_list(element->value.value.arr, NULL)); break;
            case JFH_BOOL: JFH_setboolL(celement, element->value.value.b); break;
            case JFH_NULL: JFH_setnullL(celement); break;
            default: errno = EINVAL; return NULL;
        }
        return celement;
    }
    jfh_array_t *newelement = JFH_initL();
    if (!newelement) {
        errno = ENOMEM;
        return NULL;
    }
    switch (element->value.vt) {
        case JFH_STR: JFH_setstrL_nquots(newelement, element->value.value.str.str); break;
        case JFH_INT: JFH_setintL(newelement, element->value.value.i); break;
        case JFH_DBL: JFH_setdoubleL(newelement, element->value.value.dbl); break;
        case JFH_OBJ: JFH_setobjL(newelement, JFH_copy_map(element->value.value.obj, NULL)); break;
        case JFH_LIST: JFH_setarrL(newelement, JFH_copy_list(element->value.value.arr, NULL)); break;
        case JFH_BOOL: JFH_setboolL(newelement, element->value.value.b); break;
        case JFH_NULL: JFH_setnullL(newelement); break;
        default: errno = EINVAL; return NULL;
    }
    return newelement;
}

// Copies the given map, either returns the copy or copies the whole map to another map.
jfh_obj_t *JFH_copy_map(jfh_obj_t *map, jfh_obj_t *cmap) {
    if (!map) {
        errno = EINVAL;
        return NULL;
    }
    jfh_obj_t *cur = map;
    if (cmap) {
        jfh_obj_t *ccur = cmap;
        JFH_free_pair(ccur);
        ccur = JFH_initM();
        while (cur) {
            switch (cur->value.vt) {
                case JFH_STR: JFH_setstrH_nquots(ccur, cur->key, cur->value.value.str.str); break;
                case JFH_INT: JFH_setintH(ccur, cur->key, cur->value.value.i); break;
                case JFH_DBL: JFH_setdoubleH(ccur, cur->key, cur->value.value.dbl); break;
                case JFH_OBJ: JFH_setobjH(ccur, cur->key, JFH_copy_map(cur->value.value.obj, NULL)); break;
                case JFH_LIST: JFH_setarrH(ccur, cur->key, JFH_copy_list(cur->value.value.arr, NULL)); break;
                case JFH_BOOL: JFH_setboolH(ccur, cur->key, cur->value.value.b); break;
                case JFH_NULL: JFH_setnullH(ccur, cur->key); break;
                default: errno = EINVAL; return NULL;
            }
            if (cur->next) {
                JFH_free_pair(ccur->next);
                ccur->next = JFH_initM();
                if (!ccur->next) {
                    return NULL;
                }
                ccur->next->prev = ccur;
                ccur = ccur->next;
            }
            cur = cur->next;
        }
        return cmap;
    }
    jfh_obj_t *newmap = JFH_initM();
    if (!newmap) return NULL;
    jfh_obj_t *newcur = newmap;
    while (cur) {
        switch (cur->value.vt) {
            case JFH_STR: JFH_setstrH_nquots(newcur, cur->key, cur->value.value.str.str); break;
            case JFH_INT: JFH_setintH(newcur, cur->key, cur->value.value.i); break;
            case JFH_DBL: JFH_setdoubleH(newcur, cur->key, cur->value.value.dbl); break;
            case JFH_OBJ: JFH_setobjH(newcur, cur->key, JFH_copy_map(cur->value.value.obj, NULL)); break;
            case JFH_LIST: JFH_setarrH(newcur, cur->key, JFH_copy_list(cur->value.value.arr, NULL)); break;
            case JFH_BOOL: JFH_setboolH(newcur, cur->key, cur->value.value.b); break;
            case JFH_NULL: JFH_setnullH(newcur, cur->key); break;
            default: errno = EINVAL; return NULL;
        }
        if (cur->next) {
            newcur->next = JFH_initM();
            if (!newcur->next) {
                return NULL;
            }
            newcur->next->prev = newcur;
            newcur = newcur->next;
        }
        cur = cur->next;
    }
    return newmap;
}

// Copies the given list, either returns the copy or copies the whole list to another list.
jfh_array_t *JFH_copy_list(jfh_array_t *list, jfh_array_t *clist) {
    if (!list) {
        errno = EINVAL;
        return NULL;
    }
    jfh_array_t *cur = list;
    if (clist) {
        jfh_array_t *ccur = clist;
        JFH_free_element(ccur);
        ccur = JFH_initL();
        while (cur) {
            switch (cur->value.vt) {
                case JFH_STR: JFH_setstrL_nquots(ccur, cur->value.value.str.str); break;
                case JFH_INT: JFH_setintL(ccur, cur->value.value.i); break;
                case JFH_DBL: JFH_setdoubleL(ccur, cur->value.value.dbl); break;
                case JFH_OBJ: JFH_setobjL(ccur, JFH_copy_map(cur->value.value.obj, NULL)); break;
                case JFH_LIST: JFH_setarrL(ccur, JFH_copy_list(cur->value.value.arr, NULL)); break;
                case JFH_BOOL: JFH_setboolL(ccur, cur->value.value.b); break;
                case JFH_NULL: JFH_setnullL(ccur); break;
                default: errno = EINVAL; return NULL;
            }
            if (cur->next) {
                JFH_free_element(ccur->next);
                ccur->next = JFH_initL();
                if (!ccur->next) {
                    return NULL;
                }
                ccur->next->prev = ccur;
                ccur = ccur->next;
            }
            cur = cur->next;
        }
        return clist;
    }
    jfh_array_t *newlist = JFH_initL();
    if (!newlist) return NULL;
    jfh_array_t *newcur = newlist;
    while (cur) {
        switch (cur->value.vt) {
            case JFH_STR: JFH_setstrL_nquots(newcur, cur->value.value.str.str); break;
            case JFH_INT: JFH_setintL(newcur, cur->value.value.i); break;
            case JFH_DBL: JFH_setdoubleL(newcur, cur->value.value.dbl); break;
            case JFH_OBJ: JFH_setobjL(newcur, JFH_copy_map(cur->value.value.obj, NULL)); break;
            case JFH_LIST: JFH_setarrL(newcur, JFH_copy_list(cur->value.value.arr, NULL)); break;
            case JFH_BOOL: JFH_setboolL(newcur, cur->value.value.b); break;
            case JFH_NULL: JFH_setnullL(newcur); break;
            default: errno = EINVAL; return NULL;
        }
        if (cur->next) {
            newcur->next = JFH_initL();
            if (!newcur->next) {
                return NULL;
            }
            newcur->next->prev = newcur;
            newcur = newcur->next;
        }
        cur = cur->next;
    }
    return newlist;
}

// Copies the given json value, either returns the copy or copies the json value to 2nd parameter json value
jfh_json_value_t *JFH_copy_json_value(jfh_json_value_t *val, jfh_json_value_t *cval) {
    if (!val || !cval) {
        errno = EINVAL;
        return NULL;
    }
    JFH_free_json_value(cval);
    switch (val->vt) {
        case JFH_STR: {
            cval->value.str.str = str_dup(val->value.str.str);
            if (!cval->value.str.str) {errno = ENOMEM; return NULL;}
            cval->value.str.len = val->value.str.len;
            cval->vt = JFH_STR;
            break;
        }
        case JFH_INT: {
            cval->value.i = val->value.i;
            cval->vt = JFH_INT;
            break;
        }
        case JFH_DBL: {
            cval->value.dbl = val->value.dbl;
            cval->vt = JFH_DBL;
            break;
        }
        case JFH_OBJ: {
            cval->value.obj = JFH_copy_map(val->value.obj, NULL);
            if (!cval->value.obj) {errno = ENOMEM; return NULL;}
            cval->vt = JFH_OBJ;
            break;
        }
        case JFH_LIST: {
            cval->value.arr = JFH_copy_list(val->value.arr, NULL);
            if (!cval->value.arr) {errno = ENOMEM; return NULL;}
            cval->vt = JFH_LIST;
            break;
        }
        default: errno = EINVAL; return NULL;
    }
    return cval;
}