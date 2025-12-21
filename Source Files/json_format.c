#include "..\Headers\json_format.h"

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

static int st_write(char **cur, char **str, size_t *pos, size_t *alc_n, const char *src) {
    if (!cur || !str || !pos || !alc_n || !src) {
        errno = EINVAL;
        return 1;
    }
    size_t len = strlen(src);
    while (*pos + len+1 > *alc_n) {
        *alc_n *= 2;
        if (*alc_n >= 1073741824) {
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
                char *new = evalu(curobj->value.value.str.str);
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
                char *new = evalu(curarr->value.value.str.str);
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
        if (alc_n >= 1073741824) {
            fprintf(stderr, "Memory limit (1073741824 bytes) reached\n");
            errno = ENOMEM;
            return NULL;
        }
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
        if (alc_n >= 1073741824) {
            fprintf(stderr, "Memory limit (1073741824 bytes) reached\n");
            errno = ENOMEM;
            return NULL;
        }
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
            if (alc_n >= 1073741824) {
                fprintf(stderr, "Memory limit (1073741824 bytes) reached\n");
                free(newjson);
                return NULL;
            }
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

static int stobj_parser(char *cur, jfh_obj_t **curobj) {
    if (!cur || !curobj) {
        errno = EINVAL;
        return 1;
    }
    int nest_index = 0;
    int last_brace = 1;
    bool is_string = false;
    bool is_key = true;
    bool is_obj = false;
    bool is_arr = false;
    size_t len = strlen(cur);
    char *key = malloc(len);
    char *val = malloc(len);
    if (!key || !val) {
        if (key) free(key);
        if (val) free(val);
        errno = ENOMEM;
        return 1;
    }
    char *prev = cur;
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
            if (*cur == '\"' && *prev != '\\') {
                if (is_string) {
                    is_string = false;
                } else {
                    is_string = true;
                }
            }
            if (*cur == '{' && !is_string) {
                if (*(cur+1) != '\"') goto fail;
                if (nest_index > 0 && !is_obj && !is_arr) {
                    is_obj = true;
                    last_brace = nest_index;
                }
                nest_index++;
            } else if (*cur == '}' && !is_string) {
                nest_index--;
            } else if (*cur == '[' && !is_string) {
                if (*(cur + 1) == ',') goto fail;
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
                if (*(cur + 1) == '}' || *(cur + 1) == ']') goto fail;
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
                if (*(cur + 1) == ',') goto fail;
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
            if (!newobj) goto fail;
            if (strcmp(val, "{}") != 0) {
                jfh_obj_t *newcurobj = newobj;
                if (stobj_parser(val, &newcurobj)) goto fail;
            }
            if (!JFH_setobjH(*curobj, key, newobj)) goto fail;
        } else if (is_arr) {
            jfh_array_t *newarr = JFH_initL();
            if (!newarr) goto fail;
            if (strcmp(val, "[]") != 0) {
                jfh_array_t *newcurarr = newarr;
                if (starr_parser(val, &newcurarr)) goto fail;
            }
            if (!JFH_setarrH(*curobj, key, newarr)) goto fail;
        } else {
            curval = val;
            if (
                *curval == '0' ||
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
                while (*curval) {
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
                        default: errno = JFH_EJSON; goto fail;
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
                            goto fail;
                        }
                        dbl += num;
                        while (*curval) {
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
                                default: errno = JFH_EJSON; goto fail;
                            }
                            curval++;
                        }
                        break;
                    }
                    curval++;
                }
                if (is_dbl) {
                    if (!JFH_setdoubleH(*curobj, key, dbl)) goto fail;
                } else {
                    if (!JFH_setintH(*curobj, key, num)) goto fail;
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
                        default: errno = JFH_EJSON; goto fail;
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
                            goto fail;
                        }
                        dbl += num;
                        while (*curval) {
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
                                default: errno = JFH_EJSON; goto fail;
                            }
                            curval++;
                        }
                        break;
                    }
                    curval++;
                }
                if (is_dbl) {
                    if (!JFH_setdoubleH(*curobj, key, dbl * (-1))) goto fail;
                } else {
                    if (!JFH_setintH(*curobj, key, num * (-1))) goto fail;
                }
            } else {
                if (*val != '\"') {
                    if (strcmp(val, "true") == 0) {
                        if (!JFH_setboolH(*curobj, key, true)) goto fail;
                    } else if (strcmp(val, "false") == 0) {
                        if (!JFH_setboolH(*curobj, key, false)) goto fail;
                    } else if (strcmp(val, "null") == 0) {
                        if (!JFH_setnullH(*curobj, key)) goto fail;
                    } else {
                        errno = JFH_EJSON;
                        goto fail;
                    }
                } else {
                    char *new = _evalu(val);
                    if (!new) goto fail;
                    if (!JFH_setstrH_nquots(*curobj, key, new)) goto fail;
                    free(new);
                }
            }
        }
        if (nest_index <= 0) break;
        (*curobj)->next = JFH_initM();
        if (!(*curobj)->next) goto fail;
        (*curobj)->next->prev = (*curobj);
        (*curobj) = (*curobj)->next;
    }
    free(key);
    free(val);
    return 0;
    fail:
        free(key);
        free(val);
        return 1;
}

static int starr_parser(char *cur, jfh_array_t **curarr) {
    if (!curarr || !cur) {
        errno = EINVAL;
        return 1;
    }
    int nest_index = 1;
    int last_brace = 1;
    size_t len = strlen(cur);
    char *val = malloc(len);
    if (!val) {
        errno = ENOMEM;
        return 1;
    }
    char *curval;
    char *prev = cur;
    bool is_string = false;
    bool is_obj = false;
    bool is_arr = false;
    cur++;
    while (*cur) {
        curval = val;
        is_obj = false;
        is_arr = false;
        while (*cur) {
            if (*cur == '\"' && *prev != '\\') {
                if (is_string) {
                    is_string = false;
                } else {
                    is_string = true;
                }
            }
            if (*cur == '[' && !is_string) {
                if (*(cur + 1) == ',') goto fail;
                if (nest_index > 0 && !is_arr && !is_obj) {
                    is_arr = true;
                    last_brace = nest_index;
                }
                nest_index++;
            } else if (*cur == ']' && !is_string) {
                nest_index--;
            } else if (*cur == '{' && !is_string) {
                if (*(cur + 1) != '\"') goto fail;
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
                if (*(cur + 1) == '}' || *(cur + 1) == ']') goto fail;
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
            if (!newobj) goto fail;
            if (strcmp(val, "{}") != 0) {
                jfh_obj_t *newcurobj = newobj;
                if (stobj_parser(val, &newcurobj)) goto fail;
            }
            if (!JFH_setobjL(*curarr, newobj)) goto fail;
        } else if (is_arr) {
            jfh_array_t *newarr = JFH_initL();
            if (strcmp(val, "[]") != 0) {
                jfh_array_t *newcurarr = newarr;
                if (starr_parser(val, &newcurarr)) goto fail;
            }
            if (!JFH_setarrL(*curarr, newarr)) goto fail;
        } else {
            curval = val;
            if (
                *curval == '0' ||
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
                        default: errno = JFH_EJSON; goto fail;
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
                            goto fail;
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
                                default: errno = JFH_EJSON; goto fail;
                            }
                            curval++;
                        }
                        break;
                    }
                    curval++;
                }
                if (is_dbl) {
                    if (!JFH_setdoubleL(*curarr, dbl)) goto fail;
                } else {
                    if (!JFH_setintL(*curarr, num)) goto fail;
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
                        default: errno = JFH_EJSON; goto fail;
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
                            goto fail;
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
                                default: errno = JFH_EJSON; goto fail;
                            }
                            curval++;
                        }
                        break;
                    }
                    curval++;
                }
                if (is_dbl) {
                    if (!JFH_setdoubleL(*curarr, dbl * (-1))) goto fail;
                } else {
                    if (!JFH_setintL(*curarr, num * (-1))) goto fail;
                }
            } else {
                if (*val != '\"') {
                    if (strcmp(val, "true") == 0) {
                        if (!JFH_setboolL(*curarr, true)) goto fail;
                    } else if (strcmp(val, "false") == 0) {
                        if (!JFH_setboolL(*curarr, false)) goto fail;
                    } else if (strcmp(val, "null") == 0) {
                        if (!JFH_setnullL(*curarr)) goto fail;
                    } else {
                        errno = JFH_EJSON;
                        goto fail;
                    }
                } else {
                    char *new = _evalu(val);
                    if (!new) goto fail;
                    if (!JFH_setstrL_nquots(*curarr, new)) goto fail;
                    free(new);
                }
            }
        }
        if (nest_index <= 0) break;
        (*curarr)->next = JFH_initL();
        if (!(*curarr)->next) goto fail;
        (*curarr)->next->prev = (*curarr);
        (*curarr) = (*curarr)->next;
    }
    free(val);
    return 0;
    fail:
        free(val);
        return 1;
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
    jfh_obj_t *curobj = newobj;
    if (stobj_parser(cur, &curobj)) {
        JFH_free_map(newobj);
        free(json);
        return NULL;
    }
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
    jfh_array_t *curarr = newarr;
    if (starr_parser(cur, &curarr)) {
        JFH_free_list(newarr);
        free(json);
        return NULL;
    }
    return newarr;
}