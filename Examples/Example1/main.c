#include "JSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE *file = fopen("Test.json", "w");
    obj_t *map = initM();

    setrawH(resetkey(map, "Name", 4), "\"JSON-File-Handler\"", 19);
    setrawH(resetkey(appendH(map), "Version", 7), "\"v0.9.8\"", 8);
    setrawH(resetkey(appendH(map), "Age_In_Days", 11), "null", 4);
    setrawH(resetkey(appendH(map), "isFinished", 10), "false", 5);

    char *json = encode_map(map);
    char *i_json = indent_json(json, 4, 120);
    
    fputs(i_json, file);
    free(json);
    free(i_json);
    free_map(map);
    fclose(file);
    return 0;
}