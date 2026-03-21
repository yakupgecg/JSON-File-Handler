#include <JFH.h>

/*
This example works with every type JFH can support and then encodes it.
*/

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

	JFH_setH(
		obj,
		13,
		JFH_strH("Test", "Hello, \"World\"!"),
		JFH_intH("Test2", 421),
		JFH_doubleH("Test3", 22.1409),
		JFH_boolH("Test4", true),
		JFH_nullH("Test5"),
		JFH_objH("Test6", emptyobj),
		JFH_arrH("Test7", emptyarr),
		JFH_intH("Test8", -52),
		JFH_doubleH("Test9", -26.6),
		JFH_intexpH("Test10", 10, 3),
		JFH_doubleexpH("Test11", -3.4, 2),
		JFH_objH(
			"Test12",
			JFH_setH(
				subobj,
				1,
				JFH_strH("foo", "bar")
			)
		),
		JFH_arrH(
			"Test13",
			JFH_setL(
				subarr,
				3,
				JFH_intL(1),
				JFH_intL(2),
				JFH_intL(3)
			)
		)
	);

	char *json = JFH_indent_json(JFH_encode_obj(obj), 4);
	if (!json) {
		printf("DING");
		JFH_free_map(obj);
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
	"Test10": 10e3,
	"Test11": -3.4e2,
    "Test12": {
        "foo": "bar"
    },
    "Test13": [
        1,
		2,
		3
    ]
}
    
*/