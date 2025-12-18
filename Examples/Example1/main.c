#include "JFH.h"

int main() {
	char *json = "{\"foo\": \"bar\"}";
	printf("Test json          : %s\n", json);

	jfh_obj_t *obj = JFH_parse_obj(json);
    if (!obj) return 1;
    
    printf("Key                : %s\nValue              : %s\n", obj->key, obj->value.value.str.str);
    JFH_free_map(obj);
    return 0;
}
/*
Expected output:

Test json          : {"foo": "bar"}
Key                : foo
Value              : "bar"

*/