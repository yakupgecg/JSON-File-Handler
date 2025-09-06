#include "JSON.h"
#include <stdio.h>
#include <string.h>

int CtoF(int cel) {
    return cel * 1.8 + 32;
}

int main() {
    FILE *file = fopen("Test.json", "w");
    int cel = 15;
    int fahrnt = CtoF(cel);
    char *goalstring = "{\"Celsius\": 15, \"Fahrenheit\": 59}";
    obj_t *root = resetkey(initM(), "Celsius", 7);

    setrawH(root, parseInt(cel), strlen(parseInt(cel)));
    resetkey(appendH(root), "Fahrenheit", 10);
    setrawH(pairbykey(root, "Fahrenheit"), parseInt(fahrnt), strlen(parseInt(fahrnt)));

    char *json = encode_map(root);
    
    if (strcmp(goalstring, json) == 0) {
        printf("Successful\n");
        fputs(indent_json(json, 4), file);
    } else {
        printf("Unsuccessful\n\nExpected: %s\nResult: %s\n", goalstring, json);
        free_map(root);
        fclose(file);
        return 1;
    }

    free_map(root);
    fclose(file);
    return 0;
}