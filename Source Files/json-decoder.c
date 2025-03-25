#include "..\Headers\json-decoder.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

map_t *unpackstringH(const char *str) {
    if (!str) return NULL;
    map_t *root = initM();
    unsigned int i, j = 0;
    unsigned int str_len = strlen(str);

    char *fstr = malloc(str_len + 1);

    if (!fstr) return NULL;

    for (i = 0; i < str_len; i++) {
        if (str[i] != '\n' && str[i] != '\t') {
            fstr[j++] = str[i];
        }
    }
    str_len = strlen(fstr);
    char keyseq[500];
    keyseq[0] = '\0';
    bool isstring = false;
    for (i = 0; i < str_len; i++) {
        if (fstr[i] == '\"') {
            if (fstr[i-1] != '\\') {
                if (isstring) {isstring = false;} else {isstring = true;}
            }
        }
        if (isstring == true) {
            strcat(keyseq, fstr[i]);
            // NEEDS UPDATING!
        }
    }
}