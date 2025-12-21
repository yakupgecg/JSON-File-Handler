#include <JFH.h>

/*
This example shows error handling in common cases like an excess , before } even though there is no object.
*/

int main() {
	char *json = "{\"Error?\": \"maybe\",}";
	jfh_obj_t *obj = JFH_parse_obj(json);
	if (!obj) {
		fprintf(stderr, "Parsing failed\n");
	}
	JFH_free_map(obj);
	return 0;
}

/*
Expected output:

Parsing failed

*/