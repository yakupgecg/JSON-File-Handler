#include <JFH.h>

int main() {
    char *json = "{\"Human\": {\"Height\": 170.2, \"Age\": 25}}";

    jfh_obj_t *obj = JFH_parse_obj(json);
    if (!obj) return 1;

    jfh_obj_t *age = JFH_searchH(obj, "Age");

    printf("age: %d\n", age->value.value.i);

    JFH_setintH(age, "Age", 27);

    char *encoded = JFH_indent_json(JFH_encode_obj(obj), 4);
    if (!encoded) {
        JFH_free_map(obj);
        return 1;
    }

    printf("JSON after modified: \n%s\n", encoded);

    JFH_free_map(obj);
    free(encoded);
    return 0;
}

/*
Expected output:

age: 25
JSON after modified:
{
    "Human": {
        "Height": 170.200000,
        "Age": 27
    }
}

*/