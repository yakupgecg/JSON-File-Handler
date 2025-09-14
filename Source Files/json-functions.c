#include "..\Headers\json-functions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

/* Note that functions encode_map(), encode_pair() and encode_list() have not the ability to encode very nested object or lists (due to stack overflow and recursion)
   But in that case you can turn an encoded object or an array to string and then set an object or element type to raw and then set the value to the string.
*/

// Formats a pair to JSON file format
char *encode_pair(obj_t *pair) {
    if (pair == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (pair->key == NULL) {
        errno = EINVAL;
        return NULL;
    }
    unsigned int buffersize = strlen(pair->key);  
    buffersize += 6; // For the curly braces, ": " and the keys double quotes
    switch (pair->valuetype) {
        case RAW: buffersize += strlen(pair->value) + 1; break;
        case SHR: buffersize += SHR_STR_LEN; break;
        case INT: buffersize += INT_STR_LEN; break;
        case FLT: buffersize += FLT_STR_LEN; break;
        case DBL: buffersize += DBL_STR_LEN; break;
        case LONG: buffersize += LONG_STR_LEN; break;
        case LL: buffersize += LL_STR_LEN; break;
        case LDBL: buffersize += LDBL_STR_LEN; break;
        case LIST: buffersize += list_size(pair->value); break;
        case NMAP: buffersize += map_size(pair->value); break;
        default: errno = EINVAL; return NULL;
    }
    char *buffer = malloc(buffersize);
    if (buffer == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    buffer[0] = '{';
    buffer[1] = '\0';
    strcat(buffer, "\"");
    strcat(buffer, pair->key);
    strcat(buffer, "\"");
    strcat(buffer, ": ");
    switch (pair->valuetype) {
        case RAW: strcat(buffer, (char*)pair->value); break;
        case SHR:{char srep[SHR_STR_LEN]; // srep shortened for string representation
                  sprintf(srep, "%hd", *(short*)pair->value);
                  strcat(buffer, srep);
                  break;}
        case INT:{char srep[INT_STR_LEN];
                 sprintf(srep, "%d", *(int*)pair->value);
                 strcat(buffer, srep);
                 break;}
        case FLT:{char srep[FLT_STR_LEN];
                 sprintf(srep, "%g", *(float*)pair->value);
                 strcat(buffer, srep);
                 break;}
        case DBL:{char srep[DBL_STR_LEN];
                 sprintf(srep, "%lf", *(double*)pair->value);
                 strcat(buffer, srep);
                 break;}
        case LONG:{char srep[LONG_STR_LEN];
                  sprintf(srep, "%ld", *(long*)pair->value);
                  strcat(buffer, srep);
                  break;}
        case LL:{char srep[LL_STR_LEN];
                sprintf(srep, "%lld", *(long long*)pair->value);
                strcat(buffer, srep);
                break;}
        case LDBL:{char srep[LDBL_STR_LEN];
                  sprintf(srep, "%Lf", *(long double*)pair->value);
                  strcat(buffer, srep);
                  break;}
        case LIST:{char srep[list_size(pair->value) + 2];
                  sprintf(srep, "%s", encode_list(pair->value));
                  strcat(buffer, srep);
                  break;}
        case NMAP:{char srep[map_size(pair->value) + 2];
                  sprintf(srep, "%s", encode_map(pair->value));
                  strcat(buffer, srep);
                  break;}
        default: errno = EINVAL; free(buffer); return NULL;
    }
    strcat(buffer, "}");
    return buffer;
}

// Convert a hash map structure to a JSON string
char *encode_map(obj_t *map) {
    if (map == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (map->key == NULL) {
        errno = EINVAL;
        return NULL;
    }
    unsigned int buffersize = 2;
    obj_t *current = map;
    while (current != NULL) {
        buffersize += strlen(current->key) + 4; // For the double quotes of the key and ": "
        switch (current->valuetype) {
            case RAW: buffersize += strlen(current->value) + 1; break;
            case SHR: buffersize += SHR_STR_LEN; break;
            case INT: buffersize += INT_STR_LEN; break;
            case FLT: buffersize += FLT_STR_LEN; break;
            case DBL: buffersize += DBL_STR_LEN; break;
            case LONG: buffersize += LONG_STR_LEN; break;
            case LL: buffersize += LL_STR_LEN; break;
            case LDBL: buffersize += LDBL_STR_LEN; break;
            case LIST: buffersize += list_size(current->value); break;
            case NMAP: buffersize += map_size(current->value); break;
            default: errno = EINVAL; return NULL;
        }
        current = current->next;
        if (current != NULL) {
            buffersize += 2; // For ", "
        }
    }
    char *buffer = malloc(buffersize + 1);
    if (buffer == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    buffer[0] = '{';
    buffer[1] = '\0';
    current = map;
    while (current != NULL) {
        strcat(buffer, "\"");
        strcat(buffer, current->key);
        strcat(buffer, "\"");
        strcat(buffer, ": ");
        switch (current->valuetype) {
            case RAW: strcat(buffer, (char*)current->value); break;
            case SHR:{char srep[SHR_STR_LEN]; // srep shortened for string representation
                      sprintf(srep, "%hd", *(short*)current->value);
                      strcat(buffer, srep);
                      break;}
            case INT:{char srep[INT_STR_LEN];
                     sprintf(srep, "%d", *(int*)current->value);
                     strcat(buffer, srep);
                     break;}
            case FLT:{char srep[FLT_STR_LEN];
                     sprintf(srep, "%g", *(float*)current->value);
                     strcat(buffer, srep);
                     break;}
            case DBL:{char srep[DBL_STR_LEN];
                     sprintf(srep, "%lf", *(double*)current->value);
                     strcat(buffer, srep);
                     break;}
            case LONG:{char srep[LONG_STR_LEN];
                      sprintf(srep, "%ld", *(long*)current->value);
                      strcat(buffer, srep);
                      break;}
            case LL:{char srep[LL_STR_LEN];
                    sprintf(srep, "%lld", *(long long*)current->value);
                    strcat(buffer, srep);
                    break;}
            case LDBL:{char srep[LDBL_STR_LEN];
                      sprintf(srep, "%Lf", *(long double*)current->value);
                      strcat(buffer, srep);
                      break;}
            case LIST:{char srep[list_size(current->value) + 2];
                      sprintf(srep, "%s", encode_list(current->value));
                      strcat(buffer, srep);
                      break;}
            case NMAP:{char srep[map_size(current->value) + 2];
                      sprintf(srep, "%s", encode_map(current->value));
                      strcat(buffer, srep);
                      break;}
            default: errno = EINVAL; free(buffer); return NULL;
        }
        current = current->next;
        if (current != NULL) {
            strcat(buffer, ", ");
        }
    }
    strcat(buffer, "}");
    return buffer;
}

// Converts a list to JSON file format
char *encode_list(array_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return NULL;
    }
    unsigned int buffersize = 3; // 3 for [ and ] and null terminator
    array_t *current;
    current = list;
    while (current != NULL) {
        if (current->valuetype == LIST) {
            buffersize += list_size(current->value) + 2; // updated
            current = current->next;
            continue;
        }
        switch (current->valuetype) {
            case RAW: buffersize += strlen(current->value) + 1; break;
            case SHR: buffersize += SHR_STR_LEN; break;
            case INT: buffersize += INT_STR_LEN; break;
            case FLT: buffersize += FLT_STR_LEN; break;
            case DBL: buffersize += DBL_STR_LEN; break;
            case LONG: buffersize += LONG_STR_LEN; break;
            case LL: buffersize += LL_STR_LEN; break;
            case LDBL: buffersize += LDBL_STR_LEN; break;
            case LIST: buffersize += list_size(current->value); break;
            case NMAP: buffersize += map_size(current->value); break;
            default: errno = EINVAL; return NULL;
        }
        current = current->next;
        if (current != NULL)  {
            buffersize += 2; // For ", "
        }
    }
    current = list;
    char *buffer = malloc(buffersize);
    if (buffer == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    buffer[0] = '[';
    buffer[1] = '\0';
    while (current != NULL) {
        switch (current->valuetype) {
            case RAW: strcat(buffer, (char*)current->value); break;
            case SHR:{char srep[SHR_STR_LEN];
                     sprintf(srep, "%hd", *(short*)current->value);
                     strcat(buffer, srep);
                     break;}
            case INT:{char srep[INT_STR_LEN];
                     sprintf(srep, "%d", *(int*)current->value);
                     strcat(buffer, srep);
                     break;}
            case FLT:{char srep[FLT_STR_LEN];
                     sprintf(srep, "%g", *(float*)current->value);
                     strcat(buffer, srep);
                     break;}
            case DBL:{char srep[DBL_STR_LEN];
                     sprintf(srep, "%lf", *(double*)current->value);
                     strcat(buffer, srep);
                     break;}
            case LONG:{char srep[LONG_STR_LEN];
                      sprintf(srep, "%ld", *(long*)current->value);
                      strcat(buffer, srep);
                      break;}
            case LL:{char srep[LL_STR_LEN];
                    sprintf(srep, "%lld", *(long long*)current->value);
                    strcat(buffer, srep);
                    break;}
            case LDBL:{char srep[LDBL_STR_LEN];
                      sprintf(srep, "%Lf", *(long double*)current->value);
                      strcat(buffer, srep);
                      break;}
            case LIST:{char srep[list_size(current->value) + 2];
                      sprintf(srep, "%s", encode_list(current->value));
                      strcat(buffer, srep);
                      break;}
            case NMAP:{char srep[map_size(current->value) + 2];
                      sprintf(srep, "%s", encode_map(current->value));
                      strcat(buffer, srep);
                      break;}
            default: errno = EINVAL; free(buffer); return NULL;
        }
        current = current->next;
        if (current != NULL) {
            strcat(buffer, ", ");
        }
    }
    strcat(buffer, "]");
    return buffer;
}

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