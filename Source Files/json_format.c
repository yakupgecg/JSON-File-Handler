#include "..\Headers\json_format.h"

static char *remove_whitespace(char *str) {
    if (str == NULL) {
        errno = EINVAL;
        return NULL;
    }
    char *newstr = malloc(strlen(str));
    if (newstr == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    char *newcur = newstr;
    char *cur = str;
    char *prev = str;
    *newcur = *cur;
    newcur++;
    cur++;
    bool is_string = false;
    while (*cur != '\0') {
        if (*cur == '\"') {
            if (*prev != '\\') {
                if (is_string) {
                    is_string = false;
                } else {
                    is_string = true;
                }
            }
        }
        if ((*cur != ' ' && *cur != '\n' && *cur != '\r' && *cur != '\t') || is_string) {
            *newcur = *cur;
            newcur++;
        }
        cur++;
        prev++;
    }
    *newcur = '\0';
    return newstr;
}

static int st_write(char **cur, char **str, size_t *pos, size_t *alc_n, const char *src) {
    if (!cur || !str || !pos || !alc_n || !src) {
        errno = EINVAL;
        return 1;
    }
    size_t len = strlen(src);
    while (*pos + len+1 > *alc_n) {
        *alc_n *= 2;
        if (*alc_n > 1073741824) {
            fprintf(stderr, "Memory limit (1073741824 bytes) reached\n");
            errno = ENOMEM;
            return 1;
        }
        char *temp = realloc(*str, *alc_n);
        if (!temp) {
            free(*str);
            *str = NULL;
            errno = ENOMEM;
            return 1;
        }
        *str = temp;
        *cur = *str + *pos;
    }
    memcpy(*cur, src, len);
    *cur += len;
    *pos += len;
    return 0;
}

static int stobj_encoder(obj_t *curobj, char **str, char **cur, size_t *pos, size_t *alc_n) {
    if (!curobj) {
        errno = EINVAL;
        return 1;
    }
    if (st_write(cur, str, pos, alc_n, "{")) return 1;
    while (curobj) {
        if (
            st_write(cur, str, pos, alc_n, "\"") ||
            st_write(cur, str, pos, alc_n, curobj->key) ||
            st_write(cur, str, pos, alc_n, "\"") ||
            st_write(cur, str, pos, alc_n, ":")
        ) return 1;
        switch (curobj->value.vt) {
            case STR: {   
                if (st_write(cur, str, pos, alc_n, curobj->value.value.str.str)) return 1;
                break;
            } case INT: {
                char *temp = str_Int(curobj->value.value.i);
                if(!temp) {
                    return 1;
                }
                if (st_write(cur, str, pos, alc_n, temp)) return 1;
                free(temp);
                break;
            } case DBL: {
                char *temp = str_Double(curobj->value.value.dbl);
                if(!temp) {
                    return 1;
                }
                if (st_write(cur, str, pos, alc_n, temp)) return 1;
                free(temp);
                break;
            }
            case OBJ:{
                int r = stobj_encoder(curobj->value.value.obj, str, cur, pos, alc_n);
                if (r) {
                    return 1;
                }
                break;
            }
            case LIST: {
                int r = starr_encoder(curobj->value.value.arr, str, cur, pos, alc_n);
                if (r) {
                    return 1;
                }
                break;
            }
            default: break;
        }
        curobj = curobj->next;
        if (curobj) {
            if (st_write(cur, str, pos, alc_n, ",")) return 1;
        }
    }
    if (st_write(cur, str, pos, alc_n, "}")) return 1;
    return 0;
}

static int starr_encoder(array_t *curarr, char **str, char **cur, size_t *pos, size_t *alc_n) {
    if (!curarr) {
        errno = EINVAL;
        return 1;
    }
    if (st_write(cur, str, pos, alc_n, "[")) return 1;
    while (curarr) {
        switch (curarr->value.vt) {
            case STR: {
                if (st_write(cur, str, pos, alc_n, curarr->value.value.str.str)) return 1;
                break;
            } case INT: {
                char *temp = str_Int(curarr->value.value.i);
                if(!temp) {
                    return 1;
                }
                if (st_write(cur, str, pos, alc_n, temp)) return 1;
                free(temp);
                break;
            } case DBL: {
                char *temp = str_Double(curarr->value.value.dbl);
                if(!temp) {
                    return 1;
                }
                if (st_write(cur, str, pos, alc_n, temp)) return 1;
                free(temp);
                break;
            }
            case OBJ: {
                int r = stobj_encoder(curarr->value.value.obj, str, cur, pos, alc_n);
                if (r) {
                    return 1;
                }
                break;
            }
            case LIST: {
                int r = starr_encoder(curarr->value.value.arr, str, cur, pos, alc_n);
                if (r) {
                    return 1;
                }
                break;
            }
            default: break;
        }
        curarr = curarr->next;
        if (curarr) {
            if (st_write(cur, str, pos, alc_n, ",")) return 1;
        }
    }
    if (st_write(cur, str, pos, alc_n, "]")) return 1;
    return 0;
}

char *encode_obj(obj_t *obj) {
    if (!obj) {
        errno = EINVAL;
        return NULL;
    }
    size_t alc_n = 256;
    size_t pos = 0;
    char *str = malloc(alc_n);
    if (!str) {
        errno = ENOMEM;
        return NULL;
    }
    char *cur = str;
    obj_t *curobj = obj;
    int r = stobj_encoder(curobj, &str, &cur, &pos, &alc_n);
    if (r) {
        return NULL;
    }
    if (st_write(&cur, &str, &pos, &alc_n, "\0")) return NULL;
    return str;
}

char *encode_arr(array_t *arr) {
    if (!arr) {
        errno = EINVAL;
        return NULL;
    }
    size_t alc_n = 256;
    size_t pos = 0;
    char *str = malloc(alc_n);
    if (!str) {
        errno = ENOMEM;
        return NULL;
    }
    char *cur = str;
    array_t *curarr = arr;
    int r = starr_encoder(curarr, &str, &cur, &pos, &alc_n);
    if (r) {
        return NULL;
    }
    if (st_write(&cur, &str, &pos, &alc_n, "\0")) return NULL;
    return str;
}

// Indents and formats a json string
char *indent_json(char *ajson, size_t indent_len) {
    if (ajson == NULL) {
        errno = EINVAL;
        return NULL;
    }
    char *json = remove_whitespace(ajson);
    if (json == NULL) {
        return NULL;
    }
    char *cur = json;
    if (*cur != '{' && *cur != '[') {
        errno = EJSON;
        return NULL;
    }
		
    bool is_string = false;
    size_t alc_n = 256;
    int indentation = 0;
    size_t str_len = strlen(json)+1;
    size_t len_i = 0; // How many characters are appended to newjson
    size_t nmem = 0; // Needed memory for reallocation
    int k;
    int i;
    int nest_index = 0;
    int list_index = 0;
    char *newjson = malloc(alc_n);
    if (newjson == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    char *newcur = newjson;
    for (i = 0; i < str_len; i++) {
        nmem = 0;
        if (*cur == '\"') {
            if (*(cur - 1) != '\\') {
                if (is_string) {
                    is_string = false;
                } else {
                    is_string = true;
                }
            }
        } else if (*cur == '{' && !is_string) {
            nest_index++;
            indentation++;
            nmem += indentation * indent_len + 1; // Extra 1 for the new line
        } else if (*cur == '}' && !is_string) {
            nest_index--;
            indentation--;
            nmem += indentation * indent_len + 1;
        } else if (*cur == '[' && !is_string) {
            list_index++;
            indentation++;
            nmem += indentation * indent_len + 1;
        } else if (*cur == ']' && !is_string) {
            list_index--;
            indentation--;
            nmem += indentation * indent_len + 1;
	    } else if (*cur == ':' && !is_string) {
            nmem += 1;
        } else if (*cur == ',' && !is_string) {
            nmem += indentation * indent_len + 1;
        }
        while (len_i + nmem >= alc_n) {
            alc_n *= 2;
            char *temp = realloc(newjson, alc_n);
            if (alc_n > 1073741824) {
                fprintf(stderr, "Memory limit (1073741824 bytes) reached\n");
                free(newjson);
                return NULL;
            }
            if (temp == NULL) {
                errno = ENOMEM;
                free(newjson);
                return NULL;
            }
            newjson = temp;
			newcur = newjson + len_i;
        }
        if ((*cur == '}' || *cur == ']') && !is_string) {
            *newcur = '\n';
            newcur++;
            len_i++;
            for (k = 0; k < indentation * indent_len; k++) {
                *newcur = ' ';
                newcur++;
                len_i++;
            }
        }
	    *newcur = *cur;
        if (*cur == ':' && !is_string) {
            newcur++;
	        *newcur = ' ';
	        len_i++;
	    } else if (*cur == ',' && !is_string) {
            newcur++;
            *newcur = '\n';
            len_i++;
            for (k = 0; k < indentation * indent_len; k++) {
                newcur++;
                *newcur = ' ';
                len_i++;
            }
        } else if ((*cur == '{' || *cur == '[') && !is_string) {
            newcur++;
            *newcur = '\n';
            len_i++;
            for (k = 0; k < indentation * indent_len; k++) {
                newcur++;
                *newcur = ' ';
                len_i++;
            }
        }
	    newcur++;
	    cur++;
	    len_i++;
    }
    if (list_index != 0 || nest_index != 0) {
        free(newjson);
        errno = EJSON;
        return NULL;
    }
    *newcur = '\0';
    return newjson;
}

// Decodes a json string that represents a pair
obj_t *decode_pair(char *str) {
    // Check for invalid args
    if (str == NULL) {
        errno = EINVAL;
        return NULL;
    }

    // Create object
    obj_t *newobj = initM();
    if (newobj == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    char *cur = str;
    size_t alc_i = 1;
    bool is_string = false;

    // Get the needed size for the string
    while (*cur != '\0') {
        if (*cur == '\"') {
            if (*--cur != '\\') {
                if (is_string) is_string = false; else is_string = true; 
            }
            cur++;
        }
       if (is_string || (*cur != ' ' && *cur != '\t' && *cur != '\n')) {
            alc_i++;
        }
        cur++;
    }
    cur = str;
    char *newstr = malloc(alc_i);
    if (newstr == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    char *newcur = newstr;

    // Copy the string without spaces
    while (*cur != '\0') {
        if (*cur == '\"') {
            if (is_string) is_string = false; else is_string = true; 
        } else if ((*cur == ' ' || *cur == '\n' || *cur == '\t') && !is_string) {
            cur++;
            continue;
        }
        *newcur = *cur;
        cur++;
        newcur++;
    }
    *newcur = '\0';
    newcur = newstr;
    if (*newcur != '{') {
        errno = EJSON;
        free(newstr);
        free_map(newobj);
        return NULL;
    }

    // Check that there are right amount of brackets
    is_string = false;
    newcur = newstr;
    int nest = 0;
    while (*newcur != '\0') {
        if (*newcur == '\"') {
            if (*--newcur != '\\') {
                if (is_string) is_string = false; else is_string = true;
                newcur++;
            }
            newcur++;
        }
        if (!is_string) {
            if (*newcur == '{') nest++;
            else if (*newcur == '}') nest--;
        }
        newcur++;
    }
    if (nest != 0) {
        errno = EJSON;
        free_map(newobj);
        free(newstr);
        return NULL;
    }

    // Check the appropriate quotation marks
    newcur = newstr;
    size_t quots = 0;
    while (*++newcur != '\0') {
        if (*newcur == '\"') {
            if (*--newcur != '\\') {
                quots++;
            }
            newcur++;
        }
    }
    if (quots % 2 != 0) {
        free(newstr);
        free_map(newobj);
        errno = EJSON;
        return NULL;
    }

    // Key evaluation
    newcur = newstr;
    alc_i = 0;
    size_t key_i = 0;
    while (*newcur != '\"') newcur++;
    while (*++newcur != '\0') {
        if (*newcur == '\"') {
            if (*--newcur != '\\') break;
            newcur++;
        }
        alc_i++;
    }
    char *key = malloc(alc_i);
    if (key == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    char *keycur = key;
    newcur = newstr;
    while (*newcur != '\"') newcur++;
    newcur++;
    while (key_i < alc_i) {
        *keycur = *newcur;
        keycur++;
        newcur++;
        key_i++;
    }
    *keycur = '\0';
    resetkey(newobj, key);

    // Value evaluation
    alc_i = 0;
    while (*newcur != ':') newcur++;
    newcur++;
    char *temp = newcur;
    while (*newcur != '\0') {
        if (*newcur == '\"') {
            if (*--newcur != '\\') {
                if (is_string) {
                    is_string = false;
                } else {
                    is_string = true;
                }
            }
            newcur++;
        }
        if (*newcur == '\"' && !is_string) {
            alc_i++;
            break;
        }
        alc_i++;
        newcur++;
    }
    char *val = malloc(alc_i);
    if (val == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    char *valcur = val;
    size_t val_i = 0;
    newcur = temp;
    while (val_i < alc_i) {
        *valcur = *newcur;
        valcur++;
        newcur++;
        val_i++;
    }
    *valcur = '\0';
    setstrH(newobj, NULL, val);
    free(newstr);
    return newobj;
}
