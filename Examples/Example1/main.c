#include "JSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE *file = fopen("Test.json", "w");
    array_t *map = initM();

    setrawH(resetkey(map, "Name", 4), "\"CSON\"", 6);
    setrawH(resetkey(appendH(map), "Version", 7), "\"v0.9.4\"", 8);
    setrawH(resetkey(appendH(map), "Age_In_Days", 11), "null", 4);
    setrawH(resetkey(appendH(map), "isFinished", 10), "false", 5);

    char *json = maptoJSON(map);
    fputs(json, file);
    free(json);
    freemap(map);
    fclose(file);
    return 0;
}