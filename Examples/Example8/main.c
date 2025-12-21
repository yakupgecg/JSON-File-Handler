#include <JFH.h>

/*
This example shows some error handling. The json starts with { but ends with ] instead of }, which is of course illegal in json.
*/

int main() {
	char *json = "{\"foo\": \"bar\"]";
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