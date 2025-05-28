#include "JSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	FILE *file = fopen("Test.json", "w");
	obj_t *root = initM();
	obj_t *subroot = initM();
	array_t *source_Files = initL();
	array_t *header_Files = initL();
	char *goalstring = "{\"JSON-File-Handler\": {\"Headers\": [\"structure.h\", \"parser.h\", \"json-encoder.h\", \"functions.h\", \"JSON.h\"], \"Source Files\": [\"structure.c\", \"parser.c\", \"json-encoder.c\", \"functions.c\"]}}";

	setrawL(source_Files, "\"structure.c\"", 13);
	setrawL(appendL(source_Files), "\"parser.c\"", 10);
	setrawL(appendL(source_Files), "\"json-encoder.c\"", 16);
	setrawL(appendL(source_Files), "\"functions.c\"", 13);

	setrawL(header_Files, "\"structure.h\"", 13);
	setrawL(appendL(header_Files), "\"parser.h\"", 10);
	setrawL(appendL(header_Files), "\"json-encoder.h\"", 16);
	setrawL(appendL(header_Files), "\"functions.h\"", 13);
	setrawL(appendL(header_Files), "\"JSON.h\"", 8);

	char *header_Files_json = encode_list(header_Files);
	char *source_Files_json = encode_list(source_Files);

	setrawH(resetkey(subroot, "Headers", 7), header_Files_json, strlen(header_Files_json));
	setrawH(resetkey(appendH(subroot), "Source Files", 12), source_Files_json, strlen(source_Files_json));

	char *json = encode_map(subroot);

	setrawH(resetkey(root, "JSON-File-Handler", 17), json, strlen(json));

	json = encode_map(root);

	if (strcmp(json, goalstring) == 0) {
		printf("Success!");
		fputs(json, file);
	} else {
		printf("Unsuccess!");
	}

	free(json);
	free(header_Files_json);
	free(source_Files_json);
	free_list(header_Files);
	free_list(source_Files);
	free_map(subroot);
	free_map(root);
	return 0;
}
