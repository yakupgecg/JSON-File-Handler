#include <JFH.h>

int main() {
	jfh_obj_t *obj = JFH_initM();
	jfh_obj_t *subobj = JFH_initM();
	jfh_array_t *subarr = JFH_initL();
	jfh_obj_t *emptyobj = JFH_initM();
	jfh_array_t *emptyarr = JFH_initL();
	if (!obj || !emptyobj || !emptyarr || !subobj || !subarr) {
		if (obj) JFH_free_map(obj);
		if (emptyobj) JFH_free_map(emptyobj);
		if (emptyarr) JFH_free_list(emptyarr);
		if (subobj) JFH_free_map(subobj);
		if (subarr) JFH_free_list(subarr);
		return 1;
	}

	JFH_setstrH(obj, "Test", "Hello, \"World\"!");
	JFH_setintH(JFH_appendH(obj), "Test2", 421);
	JFH_setdoubleH(JFH_appendH(obj), "Test3", 22.1409);
	JFH_setboolH(JFH_appendH(obj), "Test4", true);
	JFH_setnullH(JFH_appendH(obj), "Test5");
	JFH_setobjH(JFH_appendH(obj), "Test6", emptyobj);
	JFH_setarrH(JFH_appendH(obj), "Test7", emptyarr);
	JFH_setintH(JFH_appendH(obj), "Test8", -52);
	JFH_setdoubleH(JFH_appendH(obj), "Test9", -26.6);
	JFH_setobjH(JFH_appendH(obj), "Test10", subobj);
	JFH_setarrH(JFH_appendH(obj), "Test11", subarr);

	JFH_setstrH(subobj, "foo", "bar");

	JFH_setintL(subarr, 1);
	JFH_setintL(subarr, 2);
	JFH_setintL(subarr, 3);

	char *json = JFH_indent_json(JFH_encode_obj(obj), 4);
	if (!json) {
		JFH_free_map(obj);
		JFH_free_map(emptyobj);
		JFH_free_list(emptyarr);
		JFH_free_map(subobj);
		JFH_free_list(subarr);
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
    "Test": "Hello, \"World\"!",
    "Test2": 421,
    "Test3": 22.140900,
    "Test4": true,
    "Test5": null,
    "Test6": {},
    "Test7": [],
    "Test8": -52,
    "Test9": -26.600000,
    "Test10": {
        "foo": "bar"
    },
    "Test11": [
        3
    ]
}
    
*/