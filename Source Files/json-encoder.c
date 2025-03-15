#include "..\Headers\json-encoder.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Formats a pair to JSON file format
char *pairtoJSON(map_t *pair) {
    if (pair->key == NULL) {
        return "L-NULL";
    }
    if (pair->value == NULL) {
        pair->valuetype = RAW;
        pair->value = "null";
    }
    unsigned int buffersize = strlen(pair->key);  
    buffersize += 6; // For the curly braces, ": " and the keys double quotes
    switch (pair->valuetype) {
        case RAW: buffersize += strlen(pair->value); break;
        case SHR: buffersize += SHR_STR_LEN; break;
        case INT: buffersize += INT_STR_LEN; break;
        case FLT: buffersize += FLT_STR_LEN; break;
        case DBL: buffersize += DBL_STR_LEN; break;
        case LONG: buffersize += LONG_STR_LEN; break;
        case LL: buffersize += LL_STR_LEN; break;
        case LDBL: buffersize += LDBL_STR_LEN; break;
        case LIST: buffersize += calclistsize(pair->value); break;
        case NMAP: buffersize += calcmapsize(pair->value); break;
        default: return "VT-NULL";
    }
    char *buffer = malloc(buffersize);
    if (buffer == NULL) {
        return "malloc-NULL";
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
        case LIST:{char srep[calclistsize(pair->value) + 2];
                  sprintf(srep, "%s", listoJSON(pair->value));
                  strcat(buffer, srep);
                  break;}
        case NMAP:{char srep[calcmapsize(pair->value) + 2];
                  sprintf(srep, "%s", listoJSON(pair->value));
                  strcat(buffer, srep);
                  break;}
        default: return "VT-NULL";
    }
    strcat(buffer, "}");
    return buffer;
}

// Note that maptoJSON() function cannot encode hashmaps nested to lists that are nested to hashmaps (according to my experience)

// Convert a hash map structure to a JSON string
char *maptoJSON(map_t *map) {
    if (map == NULL) {
        return "L-NULL";
    }
    unsigned int buffersize = 2;
    map_t *current = map;
    while (current != NULL) {
        buffersize += strlen(current->key) + 4; // For the double quotes of the key and ": "
        switch (current->valuetype) {
            case RAW: buffersize += strlen(current->value); break;
            case SHR: buffersize += SHR_STR_LEN; break;
            case INT: buffersize += INT_STR_LEN; break;
            case FLT: buffersize += FLT_STR_LEN; break;
            case DBL: buffersize += DBL_STR_LEN; break;
            case LONG: buffersize += LONG_STR_LEN; break;
            case LL: buffersize += LL_STR_LEN; break;
            case LDBL: buffersize += LDBL_STR_LEN; break;
            case LIST: buffersize += calclistsize(current->value); break;
            case NMAP: buffersize += calcmapsize(current->value); break;
            default: return "VT-NULL";
        }
        current = current->next;
        if (current != NULL) {
            buffersize += 2; // For ", "
        }
    }
    char *buffer = malloc(buffersize + 1);
    if (buffer == NULL) {
        return "malloc-NULL";
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
            case LIST:{char srep[calclistsize(current->value) + 2];
                      sprintf(srep, "%s", listoJSON(current->value));
                      strcat(buffer, srep);
                      break;}
            case NMAP:{char srep[calcmapsize(current->value) + 2];
                      sprintf(srep, "%s", listoJSON(current->value));
                      strcat(buffer, srep);
                      break;}
            default: return "VT-NULL";
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
char *listoJSON(list_t *list) {
    if (list == NULL) {
        return "L-NULL";
    }
    unsigned int buffersize = 3; // 3 for [ and ] and null terminator
    list_t *current;
    current = list;
    while (current != NULL) {
        if (current->valuetype == LIST) {
            buffersize += calclistsize(current->value) + 2;
            current = current->next;
            continue;
        }
        switch (current->valuetype) {
            case RAW: buffersize += strlen(current->value); break;
            case SHR: buffersize += SHR_STR_LEN; break;
            case INT: buffersize += INT_STR_LEN; break;
            case FLT: buffersize += FLT_STR_LEN; break;
            case DBL: buffersize += DBL_STR_LEN; break;
            case LONG: buffersize += LONG_STR_LEN; break;
            case LL: buffersize += LL_STR_LEN; break;
            case LDBL: buffersize += LDBL_STR_LEN; break;
            case LIST: buffersize += calclistsize(current->value); break;
            case NMAP: buffersize += calcmapsize(current->value); break;
            default: return "VT-NULL";
        }
        current = current->next;
        if (current != NULL)  {
            buffersize += 2; // For ", "
        }
    }
    current = list;
    char *buffer = malloc(buffersize);
    if (buffer == NULL) {
        return "malloc-NULL";
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
            case LIST:{char srep[calclistsize(current->value) + 2];
                      sprintf(srep, "%s", listoJSON(current->value));
                      strcat(buffer, srep);
                      break;}
            case NMAP:{char srep[calcmapsize(current->value) + 2];
                      sprintf(srep, "%s", maptoJSON(current->value));
                      strcat(buffer, srep);
                      break;}
            default: return "VT-NULL";
        }
        current = current->next;
        if (current != NULL) {
            strcat(buffer, ", ");
        }
    }
    strcat(buffer, "]");
    return buffer;
}