#include "JFH.h"

int main() {
    jfh_obj_t *obj = JFH_initM();
    jfh_array_t *list = JFH_initL();
    if (!obj || !list) {
        if (obj) free(obj);
        if (list) free(list);
        return 1;
    }
    JFH_setintL(list, -7);
    JFH_setboolL(JFH_appendL(list), true);

    JFH_setstrH(obj, "foo", "bar");
    JFH_setarrH(obj, "Example list", list);

    char *json = JFH_indent_json(JFH_encode_obj(obj), 4);
    if (!json) {
        JFH_free_map(obj);
        JFH_free_list(list);
        return 1;
    }

    printf("Json: \n%s\n", json);
    free(json);
    JFH_free_map(obj);
    return 0;
}
/*
Expected output:

Json:
{
    "Example list": [
        -7,
        true
    ]
}

*/