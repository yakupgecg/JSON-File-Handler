#include "JFH.h"

int main() {
    FILE *file = fopen("Test.json", "w");
    char *json = 
    "{\"JSON-File-Handler\": {\"Examples\": {\"Example1\": [\"CompileMethod.txt\", \"main.c\"],\"Example2\": [\"CompileMethod.txt\", \"main.c\"],\"Example3\": [\"CompileMethod.txt\", \"main.c\",\"Test.json\"]},\"Headers\": [\"data_structures.h\", \"json_format.h\", \"numtostr.h\", \"utils.h\", \"JFH.h\"],\"Source Files\": [\"data_structures.c\", \"json_format.c\", \"numtostr.c\", \"utils.c\"]}}";
    jfh_obj_t *obj = JFH_parse_obj(json);
    char *encjson = JFH_indent_json(JFH_encode_obj(obj), 4);
    fprintf(file, "%s\n", encjson);
    fflush(file);
    printf("JSON: \n%s\n", encjson);
    free(encjson);
    JFH_free_map(obj);
    fclose(file);
    return 0;
}
/*

JSON: 
{
    "JSON-File-Handler": {
        "Examples": {
            "Example1": [
                CompileMethod.txt,
                main.c
            ],
            "Example2": [
                CompileMethod.txt,
                main.c
            ],
            "Example3": [
                CompileMethod.txt,
                main.c,
                Test.json
            ]
        },
        "Headers": [
            data_structures.h,
            json_format.h,
            numtostr.h,
            utils.h,
            JFH.h
        ],
        "Source Files": [
            data_structures.c,
            json_format.c,
            numtostr.c,
            utils.c
        ]
    }
}
*/