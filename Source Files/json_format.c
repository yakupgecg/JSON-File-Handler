#include "..\Headers\json_format.h"

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

static char *remove_whitespace(char *str) {
    if (!str) {
        errno = EINVAL;
        return NULL;
    }
    size_t quots = 0;
    int braces = 0;
    int square_brackets = 0;
    char *newstr = malloc(strlen(str) + 1);
    if (!newstr) {
        errno = ENOMEM;
        return NULL;
    }
    char *newcur = newstr;
    char *cur = str;
    char *prev = str;
    char *p_prev = str;
    bool is_string = false;
    while (*cur) {
        if (*cur == '\"') {
            if (*prev != '\\') {
                quots++;
                if (is_string) {
                    is_string = false;
                } else {
                    is_string = true;
                }
            } else {
                if (*p_prev == '\\' && *(p_prev-1) != '\\') {
                    quots++;
                    if (is_string) {
                        is_string = false;
                    } else {
                        is_string = true;
                    }
                }
            }
        }
        if ((*cur != ' ' && *cur != '\n' && *cur != '\r' && *cur != '\t') || is_string) {
            *newcur = *cur;
            newcur++;
        }
        if (*cur == '{' && !is_string) {
            braces++;
        } else if (*cur == '}' && !is_string) {
            braces--;
        } else if (*cur == '[' && !is_string) {
            square_brackets++;
        } else if (*cur == ']' && !is_string) {
            square_brackets--;
        }
        p_prev = prev;
        prev = cur;
        cur++;
    }
    if (braces || square_brackets || quots % 2 != 0) {
        free(newstr);
        errno = JFH_EJSON;
        return NULL;
    }
    *newcur = '\0';
    return newstr;
}

static int st_invalid(char *str) {
    if (!str) {
        errno = EINVAL;
        return 1;
    }
    if (strcmp(str, "true") == 0) return 0;
    else if (strcmp(str, "false") == 0) return 0;
    else if (strcmp(str, "null") == 0) return 0;
    if (*str == '\"') return 0;

    if (*str == '0') {
        if (*(str + 1) != '\0' && *(str + 1) != '.') return 1;
    }

    bool dbl = false;
    char *cur = str;
    if (*cur == '-') cur++;
    while (*cur) {
        if (
            *cur != '0' &&
            *cur != '1' &&
            *cur != '2' &&
            *cur != '3' &&
            *cur != '4' &&
            *cur != '5' &&
            *cur != '6' &&
            *cur != '7' &&
            *cur != '8' &&
            *cur != '9'
        ) {
            if (*cur == '.') {
                if (dbl) return 1;
            } else return 1;
        }
        if (*cur == '.') {
            if (dbl) return 1;
            dbl = true;
        }
        cur++;
    }
    return 0;
}

static int st_getmode(char *str) {
    if (!str) {
        errno = EINVAL;
        return -1;
    }
    if (strcmp(str, "null") == 0) return 0;
    else if (strcmp(str, "true") == 0) return 1;
    else if (strcmp(str, "false") == 0) return 2;
    char *cur = str;
    if (*cur == '\"') return 5;
    else if (*cur == '-') cur++;

    if (
        *cur == '0' ||
        *cur == '1' ||
        *cur == '2' ||
        *cur == '3' ||
        *cur == '4' ||
        *cur == '5' ||
        *cur == '6' ||
        *cur == '7' ||
        *cur == '8' ||
        *cur == '9'
    ) {
        while (*cur) {
            if (*cur == '.') {
                return 4;
            }
            if (
                *cur != '0' &&
                *cur != '1' &&
                *cur != '2' &&
                *cur != '3' &&
                *cur != '4' &&
                *cur != '5' &&
                *cur != '6' &&
                *cur != '7' &&
                *cur != '8' &&
                *cur != '9'
            ) return -1;
            cur++;
        }
        return 3;
    }
    errno = JFH_EJSON;
    return -1;
}

static double convert_num(char c) {
    switch (c) {
        case '0': return 0.0;
        case '1': return 1.0;
        case '2': return 2.0;
        case '3': return 3.0;
        case '4': return 4.0;
        case '5': return 5.0;
        case '6': return 6.0;
        case '7': return 7.0;
        case '8': return 8.0;
        case '9': return 9.0;
        case '.': return -1.0;
        case '-': return -2.0;
        default: return 3.0;
    }
}

static int stint_parse(char *str) {
    if (!str) {
        errno = EINVAL;
        return 0;
    }
    int n = 0;
    char *cur = str;
    if (*cur == '-') cur++;
    while (*cur) {
        int cn = (int)convert_num(*cur);
        if (cn == -3.0 || cn == -1.0) return 0;
        else {
            n *= 10;
            n += cn;
        }
        cur++;
    }
    return (*str == '-') ? n * (-1) : n;
}

static double stdbl_parse(char *str) {
    if (!str) {
        errno = EINVAL;
        return 0;
    }
    double n = 0.0;
    int i = 1;
    char *cur = str;
    if (*cur == '-') cur++;
    while (*cur) {
        double cn = convert_num(*cur);
        if (cn == -3.0) return 0;
        else if (cn == -1.0) {
            cur++;
            while (*cur) {
                cn = convert_num(*cur);
                i *= 10;
                n += cn / i;
                cur++;
            }
            return (*str == '-') ? n * (-1.0) : n;
        }
        else {
            n *= 10;
            n += cn;
        }
        cur++;
    }
    return (*str == '-') ? n * (-1.0) : n;
}

static int st_write(char **cur, char **str, size_t *pos, size_t *alc_n, const char *src) {
    if (!cur || !str || !pos || !alc_n || !src) {
        errno = EINVAL;
        return 1;
    }
    size_t len = strlen(src);
    while (*pos + len+1 > *alc_n) {
        *alc_n *= 2;
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

static char *evalu(char *str) {
    if (!str) {
        errno = EINVAL;
        return NULL;
    }
    char *cur = str;
    size_t alc_n = 1;
    while (*cur) {
        alc_n++;
        switch (*cur) {
            case '\n': case '\t': case '\r': case '\b': case '\f': case '\\': case '\"': alc_n++; break;
        }
        cur++;
    }
    char *newstr = malloc(alc_n);
    if (!newstr) {
        errno = ENOMEM;
        return NULL;
    }
    char *newcur = newstr;
    cur = str;
    bool is_first = true;
    bool is_last = false;
    while (*cur) {
        switch (*cur) {
            case '\n': *newcur++ = '\\'; *newcur++ = 'n'; cur++; break;
            case '\t': *newcur++ = '\\'; *newcur++ = 't'; cur++; break;
            case '\r': *newcur++ = '\\'; *newcur++ = 'r'; cur++; break;
            case '\b': *newcur++ = '\\'; *newcur++ = 'b'; cur++; break;
            case '\f': *newcur++ = '\\'; *newcur++ = 'f'; cur++; break;
            case '\\': *newcur++ = '\\'; *newcur++ = '\\'; cur++; break;
            case '\"': {
                if (!is_first && !is_last) {
                    *newcur++ = '\\'; 
                    *newcur++ = '\"'; 
                }
                cur++; 
                break;
            }
            default: *newcur = *cur; newcur++; cur++; break;
        }
        is_first = false;
        if (*(cur+1) == '\0') is_last = true;
    }
    *newcur = '\0';
    return newstr;
}

static char *_evalu(char *str) {
    if (!str) {
        errno = EINVAL;
        return NULL;
    }
    char *newstr = malloc(strlen(str) + 1);
    if (!newstr) {
        errno = ENOMEM;
        return NULL;
    }
    char *newcur = newstr;
    char *cur = str;
    while (*cur) {
        switch (*cur) {
            case '\n': case '\t': case '\r': case '\b': case '\f': goto fail;
        }
        if (*cur == '\\') {
            cur++;
            if (*cur == '\0') goto fail;
            switch (*cur) {
                case 'n': *newcur++ = '\n'; cur++; break;
                case 't': *newcur++ = '\t'; cur++; break;
                case 'r': *newcur++ = '\r'; cur++; break;
                case 'b': *newcur++ = '\b'; cur++; break;
                case 'f': *newcur++ = '\f'; cur++; break;
                case '\"': *newcur++ = '\"'; cur++; break;
                case '\\': *newcur++ = '\\'; cur++; break;
                case '/': *newcur++ = '/'; cur++; break;
                default: goto fail;
            }
            continue;
        }
        *newcur++ = *cur++;
    }
    *newcur = '\0';
    return newstr;
    fail:
        free(newstr);
        errno = JFH_EJSON;
        return NULL;
}

static int stobj_encoder(jfh_obj_t *curobj, char **str, char **cur, size_t *pos, size_t *alc_n) {
    if (!curobj) {
        errno = EINVAL;
        return 1;
    }
    if (st_write(cur, str, pos, alc_n, "{")) return 1;
    if (curobj->empty == true) {
        if (st_write(cur, str, pos, alc_n, "}")) return 1;
        return 0;
    }
    while (curobj) {
        if (
            st_write(cur, str, pos, alc_n, "\"") ||
            st_write(cur, str, pos, alc_n, curobj->key) ||
            st_write(cur, str, pos, alc_n, "\"") ||
            st_write(cur, str, pos, alc_n, ":")
        ) return 1;
        switch (curobj->value.vt) {
            case JFH_STR: {
                char *new = evalu(curobj->value.value.str);
                if (!new) return 1;
                if ( 
                    (st_write(cur, str, pos, alc_n, "\"")) ||
                    (st_write(cur, str, pos, alc_n, new)) ||
                    (st_write(cur, str, pos, alc_n, "\""))
                ) return 1;
                free(new);
                break;
            } case JFH_INT: {
                char *temp = JFH_str_Int(curobj->value.value.i);
                if(!temp) {
                    return 1;
                }
                if (st_write(cur, str, pos, alc_n, temp)) return 1;
                free(temp);
                break;
            } case JFH_DBL: {
                char *temp = JFH_str_Double(curobj->value.value.dbl);
                if(!temp) {
                    return 1;
                }
                if (st_write(cur, str, pos, alc_n, temp)) return 1;
                free(temp);
                break;
            }
            case JFH_OBJ:{
                int r = stobj_encoder(curobj->value.value.obj, str, cur, pos, alc_n);
                if (r) {
                    return 1;
                }
                break;
            }
            case JFH_LIST: {
                int r = starr_encoder(curobj->value.value.arr, str, cur, pos, alc_n);
                if (r) {
                    return 1;
                }
                break;
            } case JFH_BOOL: {
                if (st_write(cur, str, pos, alc_n, curobj->value.value.b ? "true" : "false")) return 1;
                break;
            } case JFH_NULL: {
                if (st_write(cur, str, pos, alc_n, "null")) return 1;
                break;
            }
            default: errno = JFH_EJSON; return 1;
        }
        curobj = curobj->next;
        if (curobj) {
            if (st_write(cur, str, pos, alc_n, ",")) return 1;
        }
    }
    if (st_write(cur, str, pos, alc_n, "}")) return 1;
    return 0;
}

static int starr_encoder(jfh_array_t *curarr, char **str, char **cur, size_t *pos, size_t *alc_n) {
    if (!curarr) {
        errno = EINVAL;
        return 1;
    }
    if (st_write(cur, str, pos, alc_n, "[")) return 1;
    if (curarr->empty == true) {
        if (st_write(cur, str, pos, alc_n, "]")) return 1;
        return 0;
    }
    while (curarr) {
        switch (curarr->value.vt) {
            case JFH_STR: {
                char *new = evalu(curarr->value.value.str);
                if (!new) return 1;
                if ( 
                    (st_write(cur, str, pos, alc_n, "\"")) ||
                    (st_write(cur, str, pos, alc_n, new)) ||
                    (st_write(cur, str, pos, alc_n, "\""))
                ) return 1;
                free(new);
                break;
            } case JFH_INT: {
                char *temp = JFH_str_Int(curarr->value.value.i);
                if(!temp) {
                    return 1;
                }
                if (st_write(cur, str, pos, alc_n, temp)) return 1;
                free(temp);
                break;
            } case JFH_DBL: {
                char *temp = JFH_str_Double(curarr->value.value.dbl);
                if(!temp) {
                    return 1;
                }
                if (st_write(cur, str, pos, alc_n, temp)) return 1;
                free(temp);
                break;
            }
            case JFH_OBJ: {
                int r = stobj_encoder(curarr->value.value.obj, str, cur, pos, alc_n);
                if (r) {
                    return 1;
                }
                break;
            }
            case JFH_LIST: {
                int r = starr_encoder(curarr->value.value.arr, str, cur, pos, alc_n);
                if (r) {
                    return 1;
                }
                break;
            } case JFH_BOOL: {
                if (st_write(cur, str, pos, alc_n, curarr->value.value.b ? "true" : "false")) return 1;
                break;
            } case JFH_NULL: {
                if (st_write(cur, str, pos, alc_n, "null")) return 1;
                break;
            }
            default: errno = JFH_EJSON; return 1;
        }
        curarr = curarr->next;
        if (curarr) {
            if (st_write(cur, str, pos, alc_n, ",")) return 1;
        }
    }
    if (st_write(cur, str, pos, alc_n, "]")) return 1;
    return 0;
}

// Encodes an object to a json string that represents an object
char *JFH_encode_obj(jfh_obj_t *obj) {
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
    jfh_obj_t *curobj = obj;
    int r = stobj_encoder(curobj, &str, &cur, &pos, &alc_n);
    if (r) {
        free(str);
        return NULL;
    }
    while (pos >= alc_n) {
        alc_n *= 2;
        char *temp = realloc(str, alc_n);
        if (!temp) {
            free(str);
            errno = ENOMEM;
            return NULL;
        }
        str = temp;
        cur += *str + pos;
    }
    *cur = '\0';
    return str;
}

// Encodes an array to a json string that represents an array
char *JFH_encode_arr(jfh_array_t *arr) {
    if (!arr) {
        errno = EINVAL;
        return NULL;
    }
    size_t alc_n = 256;
    size_t pos = 0;
    char *str = malloc(alc_n);
    if (!str) {
        free(str);
        errno = ENOMEM;
        return NULL;
    }
    char *cur = str;
    jfh_array_t *curarr = arr;
    int r = starr_encoder(curarr, &str, &cur, &pos, &alc_n);
    if (r) {
        return NULL;
    }
    while (pos >= alc_n) {
        alc_n *= 2;
        char *temp = realloc(str, alc_n);
        if (!temp) {
            free(str);
            errno = ENOMEM;
            return NULL;
        }
        str = temp;
        cur += *str + pos;
    }
    *cur = '\0';
    return str;
}

// Indents and formats a json string
char *JFH_indent_json(char *ajson, size_t indent_len) {
    if (!ajson) {
        errno = EINVAL;
        return NULL;
    }
    char *json = remove_whitespace(ajson);
    if (!json) {
        return NULL;
    }
    char *cur = json;
    char *prev = json;
    char *p_prev = json;
    if (*cur != '{' && *cur != '[') {
        errno = JFH_EJSON;
        return NULL;
    }
		
    bool is_string = false;
    size_t alc_n = 256;
    int indentation = 0;
    size_t JFH_str_len = strlen(json)+1;
    size_t len_i = 0;
    size_t nmem = 0;
    size_t k;
    size_t i;
    int nest_index = 0;
    int list_index = 0;
    char *newjson = malloc(alc_n);
    if (!newjson) {
        errno = ENOMEM;
        return NULL;
    }
    char *newcur = newjson;
    for (i = 0; i < JFH_str_len; i++) {
        nmem = 0;
        if (*cur == '\"') {
            if (*prev != '\\') {
                if (is_string) {
                    is_string = false;
                } else {
                    is_string = true;
                }
            } else {
                if (*p_prev == '\\' && *(p_prev-1) != '\\') {
                    if (is_string) {
                        is_string = false;
                    } else {
                        is_string = true;
                    }
                }
            }
        } else if (*cur == '{' && !is_string) {
            nest_index++;
            indentation++;
            nmem += indentation * indent_len + 1;
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
            if (!temp) {
                errno = ENOMEM;
                free(newjson);
                return NULL;
            }
            newjson = temp;
			newcur = newjson + len_i;
        }
        if ((*cur == '}' || *cur == ']') && !is_string && (*prev != '{' && *prev != '[')) {
            *newcur = '\n';
            newcur++;
            len_i++;
            for (k = 0; k < indentation * indent_len; k++) {
                *newcur = ' ';
                newcur++;
                len_i++;
            }
        }
        p_prev = prev;
        prev = cur;
	    *newcur = *cur;
        newcur++;
        if (*cur == ':' && !is_string) {
            *newcur = ' ';
            newcur++;
	        len_i++;
	    } else if (*cur == ',' && !is_string) {
            *newcur = '\n';
            newcur++;
            len_i++;
            for (k = 0; k < indentation * indent_len; k++) {
                *newcur = ' ';
                newcur++;
                len_i++;
            }
        } else if ((*cur == '{' || *cur == '[') && !is_string && (*(cur+1) != '}' && *(cur+1) != ']')) {
            *newcur = '\n';
            newcur++;
            len_i++;
            for (k = 0; k < indentation * indent_len; k++) {
                *newcur = ' ';
                newcur++;
                len_i++;
            }
        }
	    cur++;
	    len_i++;
    }
    if (list_index || nest_index) {
        free(newjson);
        errno = JFH_EJSON;
        return NULL;
    }
    *newcur = '\0';
    return newjson;
}

static int stobj_parser(char *cur, jfh_obj_t **curobj, char *keys, char *vals) {
    if (!cur || !curobj || !keys || !vals) {
        errno = EINVAL;
        return 1;
    }
    int nest_index = 0;
    int last_brace = 1;
    bool is_string = false;
    bool is_key = true;
    bool is_obj = false;
    bool is_arr = false;
    char *key = keys;
    char *val = vals;
    char *prev = cur;
    char *p_prev = cur;
    char *curkey;
    char *curval;
    while (*cur) {
        curkey = key;
        curval = val;
        is_obj = false;
        is_arr = false;
        while (*cur) {
            if (is_string && is_key) {
                *curkey = *cur;
                curkey++;
            }
            if (*cur == '\"') {
                if (*prev != '\\') {
                    if (is_string) {
                        is_string = false;
                    } else {
                        is_string = true;
                    }
                } else {
                    if (*p_prev == '\\' && *(p_prev-1) != '\\') {
                        if (is_string) {
                            is_string = false;
                        } else {
                            is_string = true;
                        }
                    }
                }
            }
            if (*cur == '{' && !is_string) {
                if (*(cur+1) != '\"' && *(cur+1) != '}') return 1;
                if (nest_index > 0 && !is_obj && !is_arr) {
                    is_obj = true;
                    last_brace = nest_index;
                }
                nest_index++;
            } else if (*cur == '}' && !is_string) {
                nest_index--;
            } else if (*cur == '[' && !is_string) {
                if (*(cur + 1) == ',') return 1;
                if (nest_index > 0 && !is_arr && !is_obj) {
                    is_arr = true;
                    last_brace = nest_index;
                }
                nest_index++;
            } else if (*cur == ']' && !is_string) {
                nest_index--;
            }
            if (nest_index <= 0) break;
            if (*cur == ',' && !is_string && last_brace >= nest_index) {
                if (*(cur + 1) == '}' || *(cur + 1) == ']' || *(cur + 1) == ',') return 1;
                is_key = true;
                prev = cur;
                cur++;
                break;
            }
            if (!is_key) {
                *curval = *cur;
                curval++;
            }
            if (*cur == ':' && !is_string) {
                if (*(cur + 1) == ',') return 1;
                is_key = false;
            }
            p_prev = prev;
            prev = cur;
            cur++;
        }
        curkey--;
        *curkey = '\0';
        *curval = '\0';
        char *rkey = str_dup(key);
        if (!rkey) return 1;
        if (is_obj) {
            jfh_obj_t *newobj = JFH_initM();
            if (!newobj) {free(rkey); return 1;}
            if (strcmp(val, "{}") != 0) {
                newobj->empty = false;
                jfh_obj_t *newcurobj = newobj;
                if (stobj_parser(val, &newcurobj, keys, vals)) {free(rkey); return 1;}
            }
            if (!JFH_setobjH(*curobj, rkey, newobj)) {free(rkey); return 1;}
        } else if (is_arr) {
            jfh_array_t *newarr = JFH_initL();
            if (!newarr) {free(rkey); return 1;}
            if (strcmp(val, "[]") != 0) {
                newarr->empty = false;
                jfh_array_t *newcurarr = newarr;
                if (starr_parser(val, &newcurarr, keys, vals)) {free(rkey); return 1;}
            }
            if (!JFH_setarrH(*curobj, rkey, newarr)) {free(rkey); return 1;}
        } else {
            if (st_invalid(val)) {free(rkey); errno = JFH_EJSON; return 1;}
            int mode = st_getmode(val);
            if (mode == -1) {free(rkey); errno = JFH_EJSON; return 1;}
            
            if (mode == 0) {
                if (!JFH_setnullH(*curobj, key)) {free(rkey); return 1;}  
            } else if (mode == 1) {
                if (!JFH_setboolH(*curobj, key, true)) {free(rkey); return 1;}
            } else if (mode == 2) {
                if (!JFH_setboolH(*curobj, key, false)) {free(rkey); return 1;}
            } else if (mode == 3) {
                int num = stint_parse(val);
                if (!JFH_setintH(*curobj, key, num)) {free(rkey); return 1;}
            } else if (mode == 4) {
                double num = stdbl_parse(val);
                if (!JFH_setdoubleH(*curobj, key, num)) {free(rkey); return 1;}
            } else if (mode == 5) {
                char *new = _evalu(val);
                if (!new) {free(rkey); return 1;}
                if (!JFH_setstrH_nquots(*curobj, key, new)) {free(rkey); return 1;}
                free(new);
            }
        }
        free(rkey);
        if (nest_index <= 0) break;
        (*curobj)->next = JFH_initM();
        if (!(*curobj)->next) return 1;
        (*curobj)->next->prev = (*curobj);
        (*curobj) = (*curobj)->next;
    }
    return 0;
}

static int starr_parser(char *cur, jfh_array_t **curarr, char *keys, char *vals) {
    if (!curarr || !cur || !keys || !vals) {
        errno = EINVAL;
        return 1;
    }
    int nest_index = 1;
    int last_brace = 1;
    char *val = vals;
    char *curval;
    char *prev = cur;
    char *p_prev = cur;
    bool is_string = false;
    bool is_obj = false;
    bool is_arr = false;
    cur++;
    while (*cur) {
        curval = val;
        is_obj = false;
        is_arr = false;
        while (*cur) {
            if (*cur == '\"') {
                if (*prev != '\\') {
                    if (is_string) {
                        is_string = false;
                    } else {
                        is_string = true;
                    }
                } else {
                    if (*p_prev == '\\' && *(p_prev-1) != '\\') {
                        if (is_string) {
                            is_string = false;
                        } else {
                            is_string = true;
                        }
                    }
                }
            }
            if (*cur == '[' && !is_string) {
                if (*(cur + 1) == ',') return 1;
                if (nest_index > 0 && !is_arr && !is_obj) {
                    is_arr = true;
                    last_brace = nest_index;
                }
                nest_index++;
            } else if (*cur == ']' && !is_string) {
                nest_index--;
            } else if (*cur == '{' && !is_string) {
                if (*(cur + 1) != '\"'  && *(cur+1) != '}') return 1;
                if (nest_index > 0 && !is_obj && !is_arr) {
                    is_obj = true;
                    last_brace = nest_index;
                }
                nest_index++;
            } else if (*cur == '}' && !is_string) {
                nest_index--;
            }
            if (nest_index <= 0) break;
            if (*cur == ',' && !is_string && last_brace >= nest_index) {
                if (*(cur + 1) == '}' || *(cur + 1) == ']' || *(cur + 1) == ',') return 1;
                prev = cur;
                cur++;
                break;
            }
            *curval = *cur;
            curval++;
            p_prev = prev;
            prev = cur;
            cur++;
        }
        *curval = '\0';
        if (is_obj) {
            jfh_obj_t *newobj = JFH_initM();
            if (!newobj) return 1;
            if (strcmp(val, "{}") != 0) {
                newobj->empty = false;
                jfh_obj_t *newcurobj = newobj;
                if (stobj_parser(val, &newcurobj, keys, vals)) return 1;
            }
            if (!JFH_setobjL(*curarr, newobj)) return 1;
        } else if (is_arr) {
            jfh_array_t *newarr = JFH_initL();
            if (!newarr) return 1;
            if (strcmp(val, "[]") != 0) {
                newarr->empty = false;
                jfh_array_t *newcurarr = newarr;
                if (starr_parser(val, &newcurarr, keys, vals)) return 1;
            }
            if (!JFH_setarrL(*curarr, newarr)) return 1;
        } else {
            if (st_invalid(val)) {errno = JFH_EJSON; return 1;}
            int mode = st_getmode(val);
            if (mode == -1) {errno = JFH_EJSON; return 1;}

            if (mode == 0) {
                if (!JFH_setnullL(*curarr)) return 1;
            } else if (mode == 1) {
                if (!JFH_setboolL(*curarr, true)) return 1;
            } else if (mode == 2) {
                if (!JFH_setboolL(*curarr, false)) return 1;
            } else if (mode == 3) {
                int num = stint_parse(val);
                if (!JFH_setintL(*curarr, num)) return 1;
            } else if (mode == 4) {
                double num = stdbl_parse(val);
                if (!JFH_setdoubleL(*curarr, num)) return 1;
            } else if (mode == 5) {
                char *new = _evalu(val);
                if (!new) return 1;
                if (!JFH_setstrL_nquots(*curarr, new)) return 1;
                free(new);
            }
        }
        if (nest_index <= 0) break;
        (*curarr)->next = JFH_initL();
        if (!(*curarr)->next) return 1;
        (*curarr)->next->prev = (*curarr);
        (*curarr) = (*curarr)->next;
    }
    return 0;
}

// Parses a json string that represents an object
jfh_obj_t *JFH_parse_obj(char *str) {
    if (!str) {
        errno = EINVAL;
        return NULL;
    }
    char *json = remove_whitespace(str);
    if (!json) {
        return NULL;
    }
    char *cur = json;
    if (*cur != '{') {
        errno = JFH_EJSON;
        free(json);
        return NULL;
    }
    jfh_obj_t *newobj = JFH_initM();
    if (!newobj) {
        errno = ENOMEM;
        free(json);
        return NULL;
    }
    if (!strcmp(json, "{}")) {
        free(json);
        return newobj;
    }
    size_t len = strlen(str);
    char *keys = malloc(len);
    char *vals = malloc(len);
    if (!keys || !vals) {
        if (keys) free(keys);
        if (vals) free(vals);
        errno = ENOMEM;
        return NULL;
    }
    jfh_obj_t *curobj = newobj;
    if (stobj_parser(cur, &curobj, keys, vals)) {
        JFH_free_map(newobj);
        free(json);
        return NULL;
    }
    free(keys);
    free(vals);
    free(json);
    return newobj;
}

// Parses a json string that represents an array
jfh_array_t *JFH_parse_arr(char *str) {
    if (!str) {
        errno = EINVAL;
        return NULL;
    }
    char *json = remove_whitespace(str);
    if (!json) {
        return NULL;
    }
    char *cur = json;
    if (*cur != '[') {
        errno = JFH_EJSON;
        free(json);
        return NULL;
    }
    jfh_array_t *newarr = JFH_initL();
    if (!newarr) {
        errno = ENOMEM;
        free(json);
        return NULL;
    }
    if (!strcmp(json, "[]")) {
        free(json);
        return newarr;
    }
    size_t len = strlen(str);
    char *keys = malloc(len);
    char *vals = malloc(len);
    if (!keys || !vals) {
        if (keys) free(keys);
        if (vals) free(vals);
        errno = ENOMEM;
        return NULL;
    }
    jfh_array_t *curarr = newarr;
    if (starr_parser(cur, &curarr, keys, vals)) {
        JFH_free_list(newarr);
        free(json);
        return NULL;
    }
    free(vals);
    free(keys);
    free(json);
    return newarr;
}