#include "..\Headers\json_format.h"

static char *remove_whitespace(char *str) {
    if (str == NULL) {
        errno = EINVAL;
        return NULL;
    }
    char *newstr = malloc(strlen(str) + 1);
    if (newstr == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    char *newcur = newstr;
    char *cur = str;
    char *prev = str;
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

static int stobj_encoder(jfh_obj_t *curobj, char **str, char **cur, size_t *pos, size_t *alc_n) {
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
            case JFH_STR: {   
                if (st_write(cur, str, pos, alc_n, curobj->value.value.str.str)) return 1;
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

static int starr_encoder(jfh_array_t *curarr, char **str, char **cur, size_t *pos, size_t *alc_n) {
    if (!curarr) {
        errno = EINVAL;
        return 1;
    }
    if (st_write(cur, str, pos, alc_n, "[")) return 1;
    while (curarr) {
        switch (curarr->value.vt) {
            case JFH_STR: {
                if (st_write(cur, str, pos, alc_n, curarr->value.value.str.str)) return 1;
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
        return NULL;
    }
    while (pos >= alc_n) {
        alc_n *= 2;
        if (alc_n >= 1073741824) {
            fprintf(stderr, "Memory limit (1073741824 bytes) reached\n");
            errno = ENOMEM;
            return NULL;
        }
        char *temp = realloc(str, alc_n);
        if (!temp) {
            errno = ENOMEM;
            return NULL;
        }
        str = temp;
        cur += *str + pos;
    }
    *cur = '\0';
    return str;
}

char *JFH_encode_arr(jfh_array_t *arr) {
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
    jfh_array_t *curarr = arr;
    int r = starr_encoder(curarr, &str, &cur, &pos, &alc_n);
    if (r) {
        return NULL;
    }
    while (pos >= alc_n) {
        alc_n *= 2;
        if (alc_n >= 1073741824) {
            fprintf(stderr, "Memory limit (1073741824 bytes) reached\n");
            errno = ENOMEM;
            return NULL;
        }
        char *temp = realloc(str, alc_n);
        if (!temp) {
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
    if (ajson == NULL) {
        errno = EINVAL;
        return NULL;
    }
    char *json = remove_whitespace(ajson);
    if (json == NULL) {
        return NULL;
    }
    char *cur = json;
    char *prev = json;
    if (*cur != '{' && *cur != '[') {
        errno = JFH_EJSON;
        return NULL;
    }
		
    bool is_string = false;
    size_t alc_n = 256;
    int indentation = 0;
    size_t JFH_str_len = strlen(json)+1;
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
    for (i = 0; i < JFH_str_len; i++) {
        nmem = 0;
        if (*cur == '\"') {
            if (*prev != '\\') {
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
        prev = newcur;
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
        } else if ((*cur == '{' || *cur == '[') && !is_string) {
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
    if (list_index != 0 || nest_index != 0) {
        free(newjson);
        errno = JFH_EJSON;
        return NULL;
    }
    *newcur = '\0';
    return newjson;
}

static int stobj_parser(char *cur, jfh_obj_t **curobj) {
    if (!cur || !curobj) {
        errno = EINVAL;
        return 1;
    }
    int nest_index = 0;
    int last_brace = 1;
    char *prev = cur;
    bool is_string = false;
    bool is_key = true;
    bool is_obj = false;
    bool is_arr = false;
    while (*cur != '\0') {
        char *key = malloc(strlen(cur));
        char *val = malloc(strlen(cur));
        if (!val || !key) {
            errno = ENOMEM;
            return 1;
        }
        char *curkey = key;
        char *curval = val;
        is_obj = false;
        is_arr = false;
        while (true) {
            if (is_string && is_key) {
                *curkey = *cur;
                curkey++;
            }
            if (*cur == '\"' && *prev != '\\') {
                if (is_string) {
                    is_string = false;
                } else {
                    is_string = true;
                }
            }
            if (*cur == '{' && !is_string) {
                if (nest_index > 0 && !is_obj && !is_arr) {
                    is_obj = true;
                    last_brace = nest_index;
                }
                nest_index++;
            } else if (*cur == '}' && !is_string) {
                nest_index--;
            } else if (*cur == '[' && !is_string) {
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
                is_key = false;
            }
            prev = cur;
            cur++;
        }
        curkey--;
        *curkey = '\0';
        *curval = '\0';
        if (is_obj) {
            jfh_obj_t *newobj = JFH_initM();
            if (!newobj) return 1;
            jfh_obj_t *newcurobj = newobj;
            if (stobj_parser(val, &newcurobj) != 0) return 1;
            if (!JFH_setobjH(*curobj, key, newobj)) return 1;
            free(val);
        } else if (is_arr) {
            jfh_array_t *newarr = JFH_initL();
            if (!newarr) return 1;
            jfh_array_t *newcurarr = newarr;
            if (starr_parser(val, &newcurarr) != 0) return 1;
            if (!JFH_setarrH(*curobj, key, newarr)) return 1;
            free(val);
        } else {
            curval = val;
            if (
                *curval == '1' ||
                *curval == '2' ||
                *curval == '3' ||
                *curval == '4' ||
                *curval == '5' ||
                *curval == '6' ||
                *curval == '7' ||
                *curval == '8' ||
                *curval == '9'
            ) {
                int num = 0;
                double dbl = 0.0;
                bool is_dbl = false;
                while (*curval != '\0') {
                    switch (*curval) {
                        case '0': num *= 10; break;
                        case '1': num *= 10; num++; break;
                        case '2': num *= 10; num += 2; break;
                        case '3': num *= 10; num += 3; break;
                        case '4': num *= 10; num += 4; break;
                        case '5': num *= 10; num += 5; break;
                        case '6': num *= 10; num += 6; break;
                        case '7': num *= 10; num += 7; break;
                        case '8': num *= 10; num += 8; break;
                        case '9': num *= 10; num += 9; break;
                        case '.': break;
                        default: errno = JFH_EJSON; return 1;
                    }
                    if (*curval == '.') {
                        long i = 1;
                        is_dbl = true;
                        curval++;
                        if (
                            *curval != '0' &&
                            *curval != '1' &&
                            *curval != '2' &&
                            *curval != '3' &&
                            *curval != '4' &&
                            *curval != '5' &&
                            *curval != '6' &&
                            *curval != '7' &&
                            *curval != '8' &&
                            *curval != '9'
                        ) {
                            errno = JFH_EJSON;
                            return 1;
                        }
                        dbl += num;
                        while (*curval != '\0') {
                            i *= 10;
                            switch (*curval) {
                                case '1': dbl += 1.0 / i; break;
                                case '2': dbl += 2.0 / i; break;
                                case '3': dbl += 3.0 / i; break;
                                case '4': dbl += 4.0 / i; break;
                                case '5': dbl += 5.0 / i; break;
                                case '6': dbl += 6.0 / i; break;
                                case '7': dbl += 7.0 / i; break;
                                case '8': dbl += 8.0 / i; break;
                                case '9': dbl += 9.0 / i; break;
                                case '0': break;
                                default: errno = JFH_EJSON; return 1;
                            }
                            curval++;
                        }
                        break;
                    }
                    curval++;
                }
                if (is_dbl) {
                    if (!JFH_setdoubleH(*curobj, key, dbl)) return 1;
                } else {
                    if (!JFH_setintH(*curobj, key, num)) return 1;
                }
            } else if (*curval == '-') {
                curval++;
                int num = 0;
                double dbl = 0.0;
                bool is_dbl = false;
                while (*curval != '\0') {
                    switch (*curval) {
                        case '0': num *= 10; break;
                        case '1': num *= 10; num++; break;
                        case '2': num *= 10; num += 2; break;
                        case '3': num *= 10; num += 3; break;
                        case '4': num *= 10; num += 4; break;
                        case '5': num *= 10; num += 5; break;
                        case '6': num *= 10; num += 6; break;
                        case '7': num *= 10; num += 7; break;
                        case '8': num *= 10; num += 8; break;
                        case '9': num *= 10; num += 9; break;
                        case '.': break;
                        default: errno = JFH_EJSON; return 1;
                    }
                    if (*curval == '.') {
                        long i = 1;
                        is_dbl = true;
                        curval++;
                        if (
                            *curval != '0' &&
                            *curval != '1' &&
                            *curval != '2' &&
                            *curval != '3' &&
                            *curval != '4' &&
                            *curval != '5' &&
                            *curval != '6' &&
                            *curval != '7' &&
                            *curval != '8' &&
                            *curval != '9'
                        ) {
                            errno = JFH_EJSON;
                            return 1;
                        }
                        dbl += num;
                        while (*curval != '\0') {
                            i *= 10;
                            switch (*curval) {
                                case '1': dbl += 1.0 / i; break;
                                case '2': dbl += 2.0 / i; break;
                                case '3': dbl += 3.0 / i; break;
                                case '4': dbl += 4.0 / i; break;
                                case '5': dbl += 5.0 / i; break;
                                case '6': dbl += 6.0 / i; break;
                                case '7': dbl += 7.0 / i; break;
                                case '8': dbl += 8.0 / i; break;
                                case '9': dbl += 9.0 / i; break;
                                case '0': break;
                                default: errno = JFH_EJSON; return 1;
                            }
                            curval++;
                        }
                        break;
                    }
                    curval++;
                }
                if (is_dbl) {
                    if (!JFH_setdoubleH(*curobj, key, dbl * (-1))) return 1;
                } else {
                    if (!JFH_setintH(*curobj, key, num * (-1))) return 1;
                }
            } else {
                if (!JFH_setstrH(*curobj, key, val)) return 1;
            }
        }
        free(key);
        if (nest_index <= 0) break;
        (*curobj)->next = JFH_initM();
        if (!(*curobj)->next) return 1;
        (*curobj)->next->prev = (*curobj);
        (*curobj) = (*curobj)->next;
    }
    if (nest_index != 0) {
        errno = JFH_EJSON;
        return 1;
    }
    return 0;
}

static int starr_parser(char *cur, jfh_array_t **curarr) {
    if (!curarr || !cur) {
        errno = EINVAL;
        return 1;
    }
    int nest_index = 1;
    int last_brace = 1;
    char *prev = cur;
    bool is_string = false;
    bool is_obj = false;
    bool is_arr = false;
    bool is_first = true;
    cur++;
    while (*cur != '\0') {
        char *val = malloc(strlen(cur));
        if (!val) {
            errno = ENOMEM;
            return 1;
        }
        char *curval = val;
        is_obj = false;
        is_arr = false;
        while (true) {
            if (*cur == '\"' && *prev != '\\') {
                if (is_string) {
                    is_string = false;
                } else {
                    is_string = true;
                }
            }
            if (*cur == '[' && !is_string) {
                if (nest_index > 0 && !is_arr && !is_obj) {
                    is_arr = true;
                    last_brace = nest_index;
                }
                nest_index++;
            } else if (*cur == ']' && !is_string) {
                nest_index--;
            } else if (*cur == '{' && !is_string) {
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
                prev = cur;
                cur++;
                break;
            }
            *curval = *cur;
            curval++;
            prev = cur;
            cur++;
        }
        *curval = '\0';
        if (is_obj) {
            jfh_obj_t *newobj = JFH_initM();
            if (!newobj) return 1;
            jfh_obj_t *newcurobj = newobj;
            if (stobj_parser(val, &newcurobj) != 0) return 1;
            if (!JFH_setobjL(*curarr, newobj)) return 1;
            free(val);
        } else if (is_arr) {
            jfh_array_t *newarr = JFH_initL();
            jfh_array_t *newcurarr = newarr;
            if (starr_parser(val, &newcurarr) != 0) return 1;
            if (!JFH_setarrL(*curarr, newarr)) return 1;
            free(val);
        } else {
            curval = val;
            if (
                *curval == '1' ||
                *curval == '2' ||
                *curval == '3' ||
                *curval == '4' ||
                *curval == '5' ||
                *curval == '6' ||
                *curval == '7' ||
                *curval == '8' ||
                *curval == '9'
            ) {
                int num = 0;
                double dbl = 0.0;
                bool is_dbl = false;
                while (*curval != '\0') {
                    switch (*curval) {
                        case '0': num *= 10; break;
                        case '1': num *= 10; num++; break;
                        case '2': num *= 10; num += 2; break;
                        case '3': num *= 10; num += 3; break;
                        case '4': num *= 10; num += 4; break;
                        case '5': num *= 10; num += 5; break;
                        case '6': num *= 10; num += 6; break;
                        case '7': num *= 10; num += 7; break;
                        case '8': num *= 10; num += 8; break;
                        case '9': num *= 10; num += 9; break;
                        case '.': break;
                        default: errno = JFH_EJSON; return 1;
                    }
                    if (*curval == '.') {
                        long i = 1;
                        is_dbl = true;
                        curval++;
                        if (
                            *curval != '0' &&
                            *curval != '1' &&
                            *curval != '2' &&
                            *curval != '3' &&
                            *curval != '4' &&
                            *curval != '5' &&
                            *curval != '6' &&
                            *curval != '7' &&
                            *curval != '8' &&
                            *curval != '9'
                        ) {
                            errno = JFH_EJSON;
                            return 1;
                        }
                        dbl += num;
                        while (*curval != '\0') {
                            i *= 10;
                            switch (*curval) {
                                case '1': dbl += 1.0 / i; break;
                                case '2': dbl += 2.0 / i; break;
                                case '3': dbl += 3.0 / i; break;
                                case '4': dbl += 4.0 / i; break;
                                case '5': dbl += 5.0 / i; break;
                                case '6': dbl += 6.0 / i; break;
                                case '7': dbl += 7.0 / i; break;
                                case '8': dbl += 8.0 / i; break;
                                case '9': dbl += 9.0 / i; break;
                                case '0': break;
                                default: errno = JFH_EJSON; return 1;
                            }
                            curval++;
                        }
                        break;
                    }
                    curval++;
                }
                if (is_dbl) {
                    if (!JFH_setdoubleL(*curarr, dbl)) return 1;
                } else {
                    if (!JFH_setintL(*curarr, num)) return 1;
                }
            } else if (*curval == '-') {
                curval++;
                int num = 0;
                double dbl = 0.0;
                bool is_dbl = false;
                while (*curval != '\0') {
                    switch (*curval) {
                        case '0': num *= 10; break;
                        case '1': num *= 10; num++; break;
                        case '2': num *= 10; num += 2; break;
                        case '3': num *= 10; num += 3; break;
                        case '4': num *= 10; num += 4; break;
                        case '5': num *= 10; num += 5; break;
                        case '6': num *= 10; num += 6; break;
                        case '7': num *= 10; num += 7; break;
                        case '8': num *= 10; num += 8; break;
                        case '9': num *= 10; num += 9; break;
                        default: errno = JFH_EJSON; return 1;
                    }
                    if (*curval == '.') {
                        long i = 1;
                        is_dbl = true;
                        curval++;
                        if (
                            *curval != '0' &&
                            *curval != '1' &&
                            *curval != '2' &&
                            *curval != '3' &&
                            *curval != '4' &&
                            *curval != '5' &&
                            *curval != '6' &&
                            *curval != '7' &&
                            *curval != '8' &&
                            *curval != '9'
                        ) {
                            errno = JFH_EJSON;
                            return 1;
                        }
                        dbl += num;
                        while (*curval != '\0') {
                            i *= 10;
                            switch (*curval) {
                                case '1': dbl += 1.0 / i; break;
                                case '2': dbl += 2.0 / i; break;
                                case '3': dbl += 3.0 / i; break;
                                case '4': dbl += 4.0 / i; break;
                                case '5': dbl += 5.0 / i; break;
                                case '6': dbl += 6.0 / i; break;
                                case '7': dbl += 7.0 / i; break;
                                case '8': dbl += 8.0 / i; break;
                                case '9': dbl += 9.0 / i; break;
                                case '0': break;
                                default: errno = JFH_EJSON; return 1;
                            }
                            curval++;
                        }
                        break;
                    }
                    curval++;
                }
                if (is_dbl) {
                    if (!JFH_setdoubleL(*curarr, dbl * (-1))) return 1;
                } else {
                    if (!JFH_setintL(*curarr, num * (-1))) return 1;
                }
            } else {
                if (!JFH_setstrL(*curarr, val)) return 1;
            }
        }
        if (nest_index <= 0) break;
        (*curarr)->next = JFH_initL();
        if (!(*curarr)->next) return 1;
        (*curarr)->next->prev = (*curarr);
        (*curarr) = (*curarr)->next;
    }
    if (nest_index != 0) {
        errno = JFH_EJSON;
        return 1;
    }
    return 0;
}

// Decodes a json string that represents a obj
jfh_obj_t *JFH_parse_obj(char *str) {
    if (!str) {
        errno = EINVAL;
        return NULL;
    }
    char *json = remove_whitespace(str);
    char *cur = json;
    if (*cur != '{') {
        errno = JFH_EJSON;
        return NULL;
    }
    jfh_obj_t *newobj = JFH_initM();
    if (!newobj) {
        errno = ENOMEM;
        return NULL;
    }
    jfh_obj_t *curobj = newobj;
    if (stobj_parser(cur, &curobj) != 0) return NULL;
    return newobj;
}

jfh_array_t *JFH_parse_arr(char *str) {
    if (!str) {
        errno = EINVAL;
        return NULL;
    }
    char *json = remove_whitespace(str);
    char *cur = json;
    if (*cur != '[') {
        errno = JFH_EJSON;
        return NULL;
    }
    jfh_array_t *newarr = JFH_initL();
    if (!newarr) {
        errno = ENOMEM;
        return NULL;
    }
    jfh_array_t *curarr = newarr;
    if (starr_parser(cur, &curarr) != 0) return NULL;
    return newarr;
}