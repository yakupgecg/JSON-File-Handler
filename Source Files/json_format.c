#include "..\Headers\json_format.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

// Indents a json string
char *indent_json(char *json, unsigned int indent, unsigned int alc_i) {
    if (json == NULL) {
        errno = EINVAL;
        return NULL;
    }
    char *newstr = malloc(alc_i);
    char *newch = newstr;
    unsigned int indent_len = 1;
    unsigned int nest_index = 1;
    unsigned int list_index = 0;
    unsigned int len = strlen(json)+1;
    bool is_string = false;
    bool is_obj = false;
    *newch = *json;
    newch++;
    *newch = '\n';
    newch++;
    for (int i = 0; i < indent; i++) {
        *newch = ' ';
        newch++;
    }
    char *cur_ch = json;
    for (int i = 1; i < len; i++) {
        is_obj = false;
        cur_ch++;
        if (*cur_ch == '"') {
            if (*--cur_ch != '\\') {
                if (is_string) {
                    is_string = false;
                } else {
                    is_string = true;
                }
            }
            cur_ch++;
        } else if (*cur_ch == ' ') {
            if (!is_string) {
                continue;
            }
        } else if (*cur_ch == '{') {
            if (!is_string) {
                if (list_index < 1) {
                    nest_index++;
                    is_obj = true;
                    *newch = '\n';
                    newch++;
                    for (int i = 0; i < indent * indent_len; i++) {
                        *newch = ' ';
                        newch++;
                    }
                }
            }
        } else if (*cur_ch == '}') {
            if (!is_string) {
                if (list_index < 1) {
                    if (indent_len < 1) {
                        free(newstr);
                        errno  = EJSON;
                        return NULL;
                    }
                    nest_index--;
                    indent_len--;
                    *newch = '\n';
                    newch++;
                    for (int i = 0; i < indent * indent_len; i++) {
                        *newch = ' ';
                        newch++;
                    }
                }
            }
        } else if (*cur_ch == '[') {
            if (!is_string) {
                list_index++;
                nest_index++;
            }
        } else if (*cur_ch == ']') {
            if (!is_string) {
                list_index--;
                nest_index--;
            }
        }
        *newch = *cur_ch;
        newch++;
        if (*cur_ch == ',') {
            if (!is_string) {
               *newch = ' ';
                newch++;
                if (list_index < 1) {
                    *newch = '\n';
                    newch++;
                    for (int i = 0; i < indent * indent_len; i++) {
                        *newch = ' ';
                        newch++;
                    }
                }
            }
        } else if (*cur_ch == ':') {
            if (!is_string) {
                *newch = ' ';
                newch++;
            }
        } else if (is_obj) {
            indent_len++;
            *newch = '\n';
            newch++;
            for (int i = 0; i < indent * indent_len; i++) {
                *newch = ' ';
                newch++;
            }
        }
    }
    if (nest_index != 0 || list_index != 0) {
        free(newstr);
        errno = EJSON;
        return NULL;
    }
    *newch = '\0';
    return newstr;
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
    resetkey(newobj, key, alc_i);

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
    setrawH(newobj, val, alc_i);
    free(newstr);
    return newobj;
}