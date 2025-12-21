#include <JFH.h>

/*
This example shows the handling of empty objects and arrays.
*/

int main() {
    jfh_obj_t *obj = JFH_initM();
    jfh_obj_t *emptyobj = JFH_initM();
    jfh_array_t *emptyarr = JFH_initL();
    if (!obj || !emptyobj || !emptyarr) {
        if (obj) JFH_free_map(obj);
        if (emptyobj) JFH_free_map(emptyobj);
        if (emptyarr) JFH_free_list(emptyarr);
        return 1;
    }

    JFH_setobjH(obj, "emptyobj", emptyobj);
    JFH_setarrH(JFH_appendH(obj), "emptyarr", emptyarr);

    char *json = JFH_indent_json(JFH_encode_obj(obj), 4);
    if (!json) {
        JFH_free_map(obj);
        JFH_free_map(emptyobj);
        JFH_free_list(emptyarr);
        return 1;
    }

    printf("JSON: \n%s\n", json);

    JFH_free_map(obj);
    free(json);
    return 0;
}

/*
Expected output:

JSON:
{
    "emptyobj": {},
    "emptyarr": []
}
    
*/