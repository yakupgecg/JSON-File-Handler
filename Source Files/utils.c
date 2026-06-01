#include "..\Headers\utils.h"

// This is a helper function since I am on windows and cannot use normal strdup()
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
        if (val->value.str) free(val->value.str);
        val->value.str = NULL;
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
    map->value.value.str = NULL;
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
    list->value.value.str = NULL;
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
        errno = ENOMEM;
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
        errno = ENOMEM;
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
            value->value.str = quots((char*)src);
            if (!value->value.str) return;
            value->vt = JFH_STR;
            break;
        }
        case JFH_INT: value->value.num.val.i = *(int64_t*)src; value->vt = JFH_INT; break;
        case JFH_DBL: value->value.num.val.dbl = *(double*)src; value->vt = JFH_DBL; break;
        case JFH_OBJ: value->value.obj = (jfh_obj_t*)src;  value->vt = JFH_OBJ; break;
        case JFH_LIST: value->value.arr = (jfh_array_t*)src; value->vt = JFH_LIST; break;
        case JFH_BOOL: value->value.b = *(bool*)src; value->vt = JFH_BOOL; break;
        case JFH_EXPI: value->value.num.val.i = *(int64_t*)src; value->vt = JFH_EXPI; break;
        case JFH_EXPD: value->value.num.val.dbl = *(double*)src; value->vt = JFH_EXPD; break;
        case JFH_NULL: {
            value->value.str = str_dup((char*)src);
            if (!value->value.str) return;
            value->vt = JFH_NULL;
            break;
        }
        default: errno = EINVAL; return;
    }
}

jfh_obj_t *JFH_setH(jfh_obj_t *obj, int count, ...) {
    if (!obj) {
        errno = EINVAL;
        return NULL;
    }
    obj->empty = false;
    va_list args;
    va_start(args, count);
    jfh_obj_t *cur = obj;

    jfh_val val;
    for (int i = 0; i < count; i++) {

        val = va_arg(args, jfh_val);
        if (!JFH_resetkey(cur, val.key)) return NULL;
        JFH_copy_json_value(&val.val, &cur->value);

        if (i+1 >= count) {
            break;
        }

        if (!cur->next) {
            JFH_appendH(cur);
        }
        cur = cur->next;
    }

    va_end(args);
    return obj;
} 

jfh_array_t *JFH_setL(jfh_array_t *arr, int count, ...) {
    if (!arr) {
        errno = EINVAL;
        return NULL;
    }
    arr->empty = false;
    va_list args;
    va_start(args, count);
    jfh_array_t *cur = arr;

    jfh_val val;
    for (int i = 0; i < count; i++) {

        val = va_arg(args, jfh_val);
        JFH_copy_json_value(&val.val, &cur->value);

        if (i+1 >= count) {
            break;
        }

        if (!cur->next) {
            JFH_appendL(cur);
        }
        cur = cur->next;
    }

    va_end(args);
    return arr;
} 

// Copies the given object, either returns the copy or copies to another object.
jfh_obj_t *JFH_copy_obj(jfh_obj_t *obj, jfh_obj_t *cobj) {
    if (!obj || !cobj) {
        errno = EINVAL;
        return NULL;
    }
    if (obj->empty) return obj;
    if (cobj) {
        switch (obj->value.vt) {
            case JFH_STR: JFH_setH(cobj, 1, JFH_strH_nquots(obj->key, obj->value.value.str)); break;
            case JFH_INT: JFH_setH(cobj, 1, JFH_intH(obj->key, obj->value.value.num.val.i)); break;
            case JFH_DBL: JFH_setH(cobj, 1, JFH_doubleH(obj->key, obj->value.value.num.val.dbl)); break;
            case JFH_OBJ: JFH_setH(cobj, 1, JFH_objH(obj->key, JFH_copy_map(obj->value.value.obj, NULL))); break;
            case JFH_LIST: JFH_setH(cobj, 1, JFH_arrH(obj->key, JFH_copy_list(obj->value.value.arr, NULL))); break;
            case JFH_BOOL: JFH_setH(cobj, 1, JFH_boolH(obj->key, obj->value.value.b)); break;
            case JFH_NULL: JFH_setH(cobj, 1, JFH_nullH(obj->key)); break;
            case JFH_EXPI: JFH_setH(cobj, 1, JFH_intexpH(obj->key, obj->value.value.num.val.i, obj->value.value.num.exp)); break;
            case JFH_EXPD: JFH_setH(cobj, 1, JFH_doubleexpH(obj->key, obj->value.value.num.val.dbl, obj->value.value.num.exp)); break;
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
        case JFH_STR: JFH_setH(newobj, 1, JFH_strH_nquots(obj->key, obj->value.value.str)); break;
        case JFH_INT: JFH_setH(newobj, 1, JFH_intH(obj->key, obj->value.value.num.val.i)); break;
        case JFH_DBL: JFH_setH(newobj, 1, JFH_doubleH(obj->key, obj->value.value.num.val.dbl)); break;
        case JFH_OBJ: JFH_setH(newobj, 1, JFH_objH(obj->key, JFH_copy_map(obj->value.value.obj, NULL))); break;
        case JFH_LIST: JFH_setH(newobj, 1, JFH_arrH(obj->key, JFH_copy_list(obj->value.value.arr, NULL))); break;
        case JFH_BOOL: JFH_setH(newobj, 1, JFH_boolH(obj->key, obj->value.value.b)); break;
        case JFH_NULL: JFH_setH(newobj, 1, JFH_nullH(obj->key)); break;
        case JFH_EXPI: JFH_setH(newobj, 1, JFH_intexpH(obj->key, obj->value.value.num.val.i, obj->value.value.num.exp)); break;
        case JFH_EXPD: JFH_setH(newobj, 1, JFH_doubleexpH(obj->key, obj->value.value.num.val.dbl, obj->value.value.num.exp)); break;
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
    if (element->empty) return element;
    if (celement) {
        switch (element->value.vt) {
            case JFH_STR: JFH_setL(celement, 1, JFH_strL_nquots(element->value.value.str)); break;
            case JFH_INT: JFH_setL(celement, 1, JFH_intL(element->value.value.num.val.i)); break;
            case JFH_DBL: JFH_setL(celement, 1, JFH_doubleL(element->value.value.num.val.dbl)); break;
            case JFH_OBJ: JFH_setL(celement, 1, JFH_objL(JFH_copy_map(element->value.value.obj, NULL))); break;
            case JFH_LIST: JFH_setL(celement, 1, JFH_arrL(JFH_copy_list(element->value.value.arr, NULL))); break;
            case JFH_BOOL: JFH_setL(celement, 1, JFH_boolL(element->value.value.b)); break;
            case JFH_NULL: JFH_setL(celement, 1, JFH_nullL()); break;
            case JFH_EXPI: JFH_setL(celement, 1, JFH_intexpL(element->value.value.num.val.i, element->value.value.num.exp)); break;
            case JFH_EXPD: JFH_setL(celement, 1, JFH_doubleexpL(element->value.value.num.val.dbl, element->value.value.num.exp)); break;
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
        case JFH_STR: JFH_setL(newelement, 1, JFH_strL_nquots(element->value.value.str)); break;
        case JFH_INT: JFH_setL(newelement, 1, JFH_intL(element->value.value.num.val.i)); break;
        case JFH_DBL: JFH_setL(newelement, 1, JFH_doubleL(element->value.value.num.val.dbl)); break;
        case JFH_OBJ: JFH_setL(newelement, 1, JFH_objL(JFH_copy_map(element->value.value.obj, NULL))); break;
        case JFH_LIST: JFH_setL(newelement, 1, JFH_arrL(JFH_copy_list(element->value.value.arr, NULL))); break;
        case JFH_BOOL: JFH_setL(newelement, 1, JFH_boolL(element->value.value.b)); break;
        case JFH_NULL: JFH_setL(newelement, 1, JFH_nullL()); break;
        case JFH_EXPI: JFH_setL(newelement, 1, JFH_intexpL(element->value.value.num.val.i, element->value.value.num.exp)); break;
        case JFH_EXPD: JFH_setL(newelement, 1, JFH_doubleexpL(element->value.value.num.val.dbl, element->value.value.num.exp)); break;
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
    if (map->empty) return map;
    jfh_obj_t *cur = map;
    if (cmap) {
        jfh_obj_t *ccur = cmap;
        JFH_free_pair(ccur);
        ccur = JFH_initM();
        while (cur) {
            switch (cur->value.vt) {
                case JFH_STR: JFH_setH(ccur, 1, JFH_strH_nquots(cur->key, cur->value.value.str)); break;
                case JFH_INT: JFH_setH(ccur, 1, JFH_intH(cur->key, cur->value.value.num.val.i)); break;
                case JFH_DBL: JFH_setH(ccur, 1, JFH_doubleH(cur->key, cur->value.value.num.val.dbl)); break;
                case JFH_OBJ: JFH_setH(ccur, 1, JFH_objH(cur->key, JFH_copy_map(cur->value.value.obj, NULL))); break;
                case JFH_LIST: JFH_setH(ccur, 1, JFH_arrH(cur->key, JFH_copy_list(cur->value.value.arr, NULL))); break;
                case JFH_BOOL: JFH_setH(ccur, 1, JFH_boolH(cur->key, cur->value.value.b)); break;
                case JFH_NULL: JFH_setH(ccur, 1, JFH_nullH(cur->key)); break;
                case JFH_EXPI: JFH_setH(ccur, 1, JFH_intexpH(cur->key, cur->value.value.num.val.i, cur->value.value.num.exp)); break;
                case JFH_EXPD: JFH_setH(ccur, 1, JFH_doubleexpH(cur->key, cur->value.value.num.val.dbl, cur->value.value.num.exp)); break;
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
            case JFH_STR: JFH_setH(newcur, 1, JFH_strH_nquots(cur->key, cur->value.value.str)); break;
            case JFH_INT: JFH_setH(newcur, 1, JFH_intH(cur->key, cur->value.value.num.val.i)); break;
            case JFH_DBL: JFH_setH(newcur, 1, JFH_doubleH(cur->key, cur->value.value.num.val.dbl)); break;
            case JFH_OBJ: JFH_setH(newcur, 1, JFH_objH(cur->key, JFH_copy_map(cur->value.value.obj, NULL))); break;
            case JFH_LIST: JFH_setH(newcur, 1, JFH_arrH(cur->key, JFH_copy_list(cur->value.value.arr, NULL))); break;
            case JFH_BOOL: JFH_setH(newcur, 1, JFH_boolH(cur->key, cur->value.value.b)); break;
            case JFH_NULL: JFH_setH(newcur, 1, JFH_nullH(cur->key)); break;
            case JFH_EXPI: JFH_setH(newcur, 1, JFH_intexpH(cur->key, cur->value.value.num.val.i, cur->value.value.num.exp)); break;
            case JFH_EXPD: JFH_setH(newcur, 1, JFH_doubleexpH(cur->key, cur->value.value.num.val.dbl, cur->value.value.num.exp)); break;
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
    if (list->empty) return list;
    jfh_array_t *cur = list;
    if (clist) {
        jfh_array_t *ccur = clist;
        JFH_free_element(ccur);
        ccur = JFH_initL();
        while (cur) {
            switch (cur->value.vt) {
                case JFH_STR: JFH_setL(ccur, 1, JFH_strL_nquots(cur->value.value.str)); break;
                case JFH_INT: JFH_setL(ccur, 1, JFH_intL(cur->value.value.num.val.i)); break;
                case JFH_DBL: JFH_setL(ccur, 1, JFH_doubleL(cur->value.value.num.val.dbl)); break;
                case JFH_OBJ: JFH_setL(ccur, 1, JFH_objL(JFH_copy_map(cur->value.value.obj, NULL))); break;
                case JFH_LIST: JFH_setL(ccur, 1, JFH_arrL(JFH_copy_list(cur->value.value.arr, NULL))); break;
                case JFH_BOOL: JFH_setL(ccur, 1, JFH_boolL(cur->value.value.b)); break;
                case JFH_NULL: JFH_setL(ccur, 1, JFH_nullL()); break;
                case JFH_EXPI: JFH_setL(ccur, 1, JFH_intexpL(cur->value.value.num.val.i, cur->value.value.num.exp)); break;
                case JFH_EXPD: JFH_setL(ccur, 1, JFH_doubleexpL(cur->value.value.num.val.dbl, cur->value.value.num.exp)); break;
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
            case JFH_STR: JFH_setL(newcur, 1, JFH_strL_nquots(cur->value.value.str)); break;
            case JFH_INT: JFH_setL(newcur, 1, JFH_intL(cur->value.value.num.val.i)); break;
            case JFH_DBL: JFH_setL(newcur, 1, JFH_doubleL(cur->value.value.num.val.dbl)); break;
            case JFH_OBJ: JFH_setL(newcur, 1, JFH_objL(JFH_copy_map(cur->value.value.obj, NULL))); break;
            case JFH_LIST: JFH_setL(newcur, 1, JFH_arrL(JFH_copy_list(cur->value.value.arr, NULL))); break;
            case JFH_BOOL: JFH_setL(newcur, 1, JFH_boolL(cur->value.value.b)); break;
            case JFH_NULL: JFH_setL(newcur, 1, JFH_nullL()); break;
            case JFH_EXPI: JFH_setL(newcur, 1, JFH_intexpL(cur->value.value.num.val.i, cur->value.value.num.exp)); break;
            case JFH_EXPD: JFH_setL(newcur, 1, JFH_doubleexpL(cur->value.value.num.val.dbl, cur->value.value.num.exp)); break;
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
    switch (val->vt) {
        case JFH_STR: {
            cval->value.str = str_dup(val->value.str);
            if (!cval->value.str) {errno = ENOMEM; return NULL;}
            cval->vt = JFH_STR;
            break;
        }
        case JFH_INT: {
            cval->value.num.val.i = val->value.num.val.i;
            cval->vt = JFH_INT;
            break;
        }
        case JFH_DBL: {
            cval->value.num.val.dbl = val->value.num.val.dbl;
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
        case JFH_BOOL: {
            cval->value.b = val->value.b;
            cval->vt = JFH_BOOL;
            break;
        }
        case JFH_NULL: {
            JFH_setval(cval, "null", JFH_NULL);
            break;
        }
        case JFH_EXPI: {
            cval->value.num.val.i = val->value.num.val.i;
            cval->value.num.exp = val->value.num.exp;
            cval->vt = JFH_EXPI;
            break;
        }
        case JFH_EXPD: {
            cval->value.num.val.dbl = val->value.num.val.dbl;
            cval->value.num.exp = val->value.num.exp;
            cval->vt = JFH_EXPD;
            break;
        }
        default: errno = EINVAL; return NULL;
    }
    return cval;
}

jfh_val JFH_strH(char *key, char *str) {
    jfh_val v;
    v.key = str_dup(key);
    v.val.value.str = quots(str);
    v.val.vt = JFH_STR;

    if (!key || !str) errno = EINVAL;
    
    return v;
}

jfh_val JFH_strL(char *str) {
    jfh_val v;
    v.key = NULL;
    v.val.value.str = quots(str);
    v.val.vt = JFH_STR;

    if (!str) errno = EINVAL;
    
    return v;
}

jfh_val JFH_strH_nquots(char *key, char *str) {
    jfh_val v;
    v.key = str_dup(key);
    v.val.value.str = str_dup(str);
    v.val.vt = JFH_STR;

    if (!key || !str) errno = EINVAL;
    
    return v;
}

jfh_val JFH_strL_nquots(char *str) {
    jfh_val v;
    v.key = NULL;
    v.val.value.str = str_dup(str);
    v.val.vt = JFH_STR;

    if (!str) errno = EINVAL;
    
    return v;
}

jfh_val JFH_intH(char *key, int64_t num) {
    jfh_val v;
    v.key = str_dup(key);
    v.val.value.num.val.i = num;
    v.val.vt = JFH_INT;

    if (!key) errno = EINVAL;
    
    return v;
}

jfh_val JFH_intL(int64_t num) {
    jfh_val v;
    v.key = NULL;
    v.val.value.num.val.i = num;
    v.val.vt = JFH_INT;
    
    return v;
}

jfh_val JFH_intexpH(char *key, int64_t num, int32_t exp) {
    jfh_val v;
    v.key = str_dup(key);
    v.val.value.num.val.i = num;
    v.val.value.num.exp = exp;
    v.val.vt = JFH_EXPI;

    if (!key) errno = EINVAL;
    
    return v;
}

jfh_val JFH_intexpL(int64_t num, int32_t exp) {
    jfh_val v;
    v.key = NULL;
    v.val.value.num.val.i = num;
    v.val.value.num.exp = exp;
    v.val.vt = JFH_EXPI;
    
    return v;
}

jfh_val JFH_doubleH(char *key, double num) {
    jfh_val v;
    v.key = str_dup(key);
    v.val.value.num.val.dbl = num;
    v.val.vt = JFH_DBL;

    if (!key) errno = EINVAL;
    
    return v;
}

jfh_val JFH_doubleL(double num) {
    jfh_val v;
    v.key = NULL;
    v.val.value.num.val.dbl = num;
    v.val.vt = JFH_DBL;
    
    return v;
}

jfh_val JFH_doubleexpH(char *key, double num, int32_t exp) {
    jfh_val v;
    v.key = str_dup(key);
    v.val.value.num.val.dbl = num;
    v.val.value.num.exp = exp;
    v.val.vt = JFH_EXPD;

    if (!key) errno = EINVAL;
    
    return v;
}

jfh_val JFH_doubleexpL(double num, int32_t exp) {
    jfh_val v;
    v.key = NULL;
    v.val.value.num.val.dbl = num;
    v.val.value.num.exp = exp;
    v.val.vt = JFH_EXPD;
    
    return v;
}

jfh_val JFH_objH(char *key, jfh_obj_t *obj) {
    jfh_val v;
    v.key = str_dup(key);
    v.val.value.obj = obj;
    v.val.vt = JFH_OBJ;

    if (!obj || !key) errno = EINVAL;
    
    return v;
}

jfh_val JFH_objL(jfh_obj_t *obj) {
    jfh_val v;
    v.key = NULL;
    v.val.value.obj = obj;
    v.val.vt = JFH_OBJ;

    if (!obj) errno = EINVAL;
    
    return v;
}

jfh_val JFH_arrH(char *key, jfh_array_t *arr) {
    jfh_val v;
    v.key = str_dup(key);
    v.val.value.arr = arr;
    v.val.vt = JFH_LIST;

    if (!key || !arr) errno = EINVAL;
    
    return v;
}

jfh_val JFH_arrL(jfh_array_t *arr) {
    jfh_val v;
    v.key = NULL;
    v.val.value.arr = arr;
    v.val.vt = JFH_LIST;

    if (!arr) errno = EINVAL;
    
    return v;
}

jfh_val JFH_boolH(char *key, bool b) {
    jfh_val v;
    v.key = str_dup(key);
    v.val.value.b = b;
    v.val.vt = JFH_BOOL;

    if (!key) errno = EINVAL;
    
    return v;
}

jfh_val JFH_boolL(bool b) {
    jfh_val v;
    v.key = NULL;
    v.val.value.b = b;
    v.val.vt = JFH_BOOL;
    
    return v;
}

jfh_val JFH_nullH(char *key) {
    jfh_val v;
    v.key = str_dup(key);
    v.val.value.str = str_dup("null");
    v.val.vt = JFH_NULL;

    if (!key) errno = EINVAL;
    
    return v;
}

jfh_val JFH_nullL() {
    jfh_val v;
    v.key = NULL;
    v.val.value.str = str_dup("null");
    v.val.vt = JFH_NULL;
    
    return v;
}