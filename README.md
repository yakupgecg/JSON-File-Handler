# JSON-File-Handler
A JSON library built entirely from C.

## What is JSON? (Read if you don't know)
**JSON** is shorthand for JavaScript object notation. It is a lightweight data interchange method usually associated with APIs.
For example if you want the current weather in a certain city, you do a HTTP request to a server that can provide the weather, the response
is usually something like:

```json

{
    "City": "Istanbul",
    "Temperature": {
        "Celsius": 39,
        "Fahrenheit": 102.2
    },
    "Sunny": true,
    "Raining": false,
}

```

The data inside {} are called objects and can store a key-value pair. They look like:

```json
{"key": "value"}
```

In this example arrays are not directly shown, but they are inside [] and only have a value,
no key. And they look like:

```json
["value", null, false, 1, 5.5]
```

A key can only be a string, but a value can be a string, integer, double, boolean, null, object or array.

Nested objects or arrays, have another object or array in their value, looks like this:

```json
{"Example object": [1, 2, {"object2": "value"}]}
```

Technically they can be nested infinitely but it completely depends on how much you have memory available.

But we can't get data straight out from a JSON string, and that is why we need json parsers. They turn a json string into a datatype that the machine understands.
But you probably wonder how did the weather service turn data into JSON strings, that's where encoders come in, they turn data into a json string.

More information about json in this link: http://www.json.org/

## What is JFH

JSON-File-Handler (Also **JFH**) is meant to be a JSON library for basic work with json. But I wouldn't say this is on the "high level" yet, since
things like unicode & scientific notation are missing, sadly.
Although this is "JSON"-File-Handler, this can also behave as the datatype map, which it was originally meant to be.

### Features

JFH supports almost all features except a few.

#### Supported features
* Nested objects and arrays
* Empty objects and arrays
* Strings
* Integers and doubles
* Negative integers and doubles
* Booleans
* Null values
* Parsing
* Encoding
* Indenting

Basically everything needed for basic JSON work, but this isn't on the high level, yet.

#### Unsupported features
* Unicode
* Exponents
* Scientific notation

These features are about to be added later since they are rarely used.
Exponents or scientific notations aren't as hard as Unicode which needs the support for hexadecimal numbers and a lookup table
for parsing.

### Structs

There are 3 structs in the library: `jfh_obj_t`, `jfh_array_t` and `jfh_json_value_t`

#### jfh_obj_t

These represent a JSON object.

```json
{"key": "value"}
```

It's struct looks like:

```c
typedef struct jfh_Hmap {
    char *key;
    jfh_json_value_t value;
    struct jfh_Hmap *next;
    struct jfh_Hmap *prev;
    bool empty;
} jfh_obj_t;
```

You might wonder why empty exist, and that is for handling empty structures. Sometimes there might be a nested object or an array that doesn't have anything ({} or []). For example:

```json
{"emptyobject": {}}
```


#### jfh_array_t

These represent a JSON array.

```json
[1, 2, 3]
```

And it's struct looks like:

```c
typedef struct jfh_Linkedlist {
    jfh_json_value_t value;
    struct jfh_Linkedlist *next;
    struct jfh_Linkedlist *prev;
    bool empty;
} jfh_array_t;
```

#### jfh_json_value_t

These represent a value, value more flexible than a normal void*.

It's struct looks like:
```c
typedef struct {
    enum jfh_valuetype vt;
    union {
        struct {
            char *str;
            size_t len;
        } str;
        int i;
        double dbl;
        bool b;
        jfh_obj_t *obj;
        jfh_array_t *arr;
    } value;
} jfh_json_value_t;
```

jfh_valuetype can be one of 7 valuetypes:                                                               
JFH_STR,    String  
JFH_INT,    Integer  
JFH_DBL,    Double  
JFH_LIST,   Nested list  
JFH_OBJ,    Nested map  
JFH_BOOL,   Boolean  
JFH_NULL    Null (can also be strings without quotes, but it is impractical and illegal in json)  

### Important functions

Here is a few important notes:
1. Functions ending with H or M are for objects and functions ending with L are for arrays.
2. The setter functions sets the value depending on the type, so there are setstr, setint, setdouble, setbool, setnull, setobj and setarr, of course starting with JFH_ and ending with H or L.
3. All of the setter functions use the `void setval(jfh_json_value_t *value, void *src, enum jfh_valuetype vt)`. So if you want to edit at the lowest level, use that then, but make sure it's valid.
4. Nearly all of the function return some kind of error code. Sometimes it might seem like it doesn't set errno, but that's with another function that already changes the errno.

The most important functions: 

* `jfh_obj_t *JFH_initM()`: Initializes a new object with every property set to null and returns a pointer to the object.
* `jfh_array_t *JFH_initL()`: Initializes a new array with every property set to null and returns a pointer to the array.
* `jfh_obj_t *JFH_setstrH(jfh_obj_t *obj, char *key, char *string)`: Sets an object's key to *key and value to *string and returns a pointer to the changed objects.
* `jfh_array_t *JFH_setstrL(jfh_obj_t *obj, char *string)`: Works the same way as JFH_setstrH().
* `jfh_obj_t *JFH_appendH(jfh_obj_t *obj)`: Appends an object to a map and returns a pointer to it. Also a variant for arrays JFH_appendL().
* `jfh_obj_t *JFH_popH(jfh_obj_t *obj)`: Takes the last element of the map and disconnects it from the map and returns a pointer to the deleted object.
* `jfh_obj_t *JFH_parse_obj(char *json)`: Parses the given json into a map and returns a pointer to it. Has a variant for arrays JFH_parse_arr(). Remember to free it using JFH_free_map() or JFH_free_list().
* `char *JFH_encode_obj(jfh_obj_t *object)`: Encodes a map to an allocated string, and returns a pointer to it. Remember to free it.
* `char *indent_json(char *json, size_t indent)`: Indents a json string and returns a pointer to the indented string. It's allocated so remember to free it.
* `int JFH_free_map(jfh_obj_t *object)`: Frees the object and everything after it.
* `int JFH_free_list(jfh_array_t *list)`: Frees the array and everything after it.

### Macros

The only macro is **JFH_EJSON**, which is used by functions `JFH_parse_obj/arr`, `JFH_encode_obj/arr` and `indent_json`. It's used when there's something wrong with the given json string or structure. And if so, errno is set to JFH_EJSON

## Using JFH

Using JFH is really easy. The compiling is just a bit annoying but that's nothing to worry about.

### Including the header

```c
#include "JFH.h"
/* And if you set an include path for the JFH.h you can write the JFH.h with <>.
```

JFH.h includes the following system libraries:

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
```

So you don't need to include these libraries if you use JFH.h

### Examples

Using this lib is pretty straightforward. Most of the time you use functions. Sometimes if you don't check a function failures
and use the object or element later, it might give a segmentation fault depending on the function. 

Every one of the following examples are tested so no need to worry testing at your own computer.

#### Objects

Like I mentioned earlier, objects are basically key-value pairs.

Here is some examples, I'll explain them.

##### Encoding

Example with an object:

```c
#include <JFH.h>

int main() {
    jfh_obj_t *obj = JFH_initM();

    if (!obj) return 1;

    JFH_setstrH(obj, "foo", "bar")
    JFH_setintH(JFH_appendH(obj), "Example", 5)

    char *json = JFH_encode_obj(obj);
    if (!json) return 1;

    char *indented_json = JFH_indent_json(json, 4);
    if (!indented_json) return 1;

    printf("JSON: \n%s\n", indented_json);

    JFH_free_map(obj);
    free(json);
    free(indented_json);
    return 0;
}
```  

Output:  
```json
{  
    "foo": "bar",  
    "Example": 5  
}
```


Example with nested objects:

```c
#include <JFH.h>

int main() {
    jfh_obj_t *obj = JFH_initM();
    jfh_obj_t *subobj = JFH_initM();

    if (!obj || !subobj) return 1;

    JFH_setobjH(obj, "foo", subobj);

    JFH_setdoubleH(subobj, "ExampleDbl", 3.141);
    JFH_setintH(JFH_appendH(subobj), "ExampleNum", 123);

    char *json = JFH_indent_json(JFH_encode_obj(obj), 4);
    if (!json) return 1;

    printf("JSON: \n%s\n\n", json);

    free(json);
    JFH_free_map(obj);
    return 0;
}
```  

Output:  
```json
{  
    "foo" {  
        "ExampleDbl": 3.141000,  
        "ExampleNum": 123  
    }  
}
```


##### Parsing

```c
#include <JFH.h>

int main() {
    char *json = "{\"Address1\": 1001, \"Address2\": 9221, \"Address3\": 2835}";

    jfh_obj_t *obj = JFH_parse_obj(json);
    if (!obj) return 1;

    char *encoded = JFH_indent_json(JFH_encode_obj(obj), 4);
    if (!encoded) return 1;

    printf("Parsed json: \n%s\n", encoded);

    JFH_free_map(obj);
    free(encoded);
}
```  

Output:  
```json
{  
    "Address1": 1001,  
    "Address2": 9221,  
    "Address3": 2835  
}
```


```c
#include <JFH.h>

int main() {
    char *json = "{\"Random city\": {\"Street1\": {\"Address1\": 521, \"Address2\": 859, \"Address3\": 210}, \"Street2\": {\"Address1\": 592, \"Address2\": 911}, \"Street3\": {\"Address1\": 110, \"Address2\": 219, \"Address3\": 251}}}"

    jfh_obj_t *obj = JFH_parse_obj(json);
    if (!obj) return 1;
    
    char *encoded = JFH_indent_json(JFH_encode_obj(obj), 4);
    if (!encoded) = return 1;

    printf("JSON: \n%s\n", encoded);

    JFH_free_map(obj);
    free(json);
}
```  

Output:  
```json
JSON:  
{  
    "Random city": {  
        "Street1": {  
            "Address1": 521,  
            "Address2": 859,  
            "Address3": 210  
        },  
        "Street2": {  
            "Address1": 592,  
            "Address2": 911  
        },  
        "Street3": {  
            "Address1": 110,  
            "Address2": 219,  
            "Address3": 251  
        }  
    }  
}
```


#### Arrays

Like mentioned earlier, they are value arrays.

Here is some examples for arrays.

##### Encoding

```c
#include <JFH.h>

int main() {
    jfh_array_t *elm = JFH_initL();
    if (!elm) return 1;

    JFH_setstrL(elm, "foo");
    JFH_setstrL(JFH_appendL(elm), "bar");
    JFH_setboolL(JFH_appendL(elm), true);
	JFH_setintL(JFH_appendL(elm), 3);

    char *json = JFH_indent_json(JFH_encode_arr(elm), 4);
    if (!json) return 1;

    printf("JSON: \n%s\n", json);

    JFH_free_list(elm);
    free(json);
    return 0;
}
```  

Output:  
```json
JSON:  
[  
    "foo",  
    "bar",  
    true,  
    3  
]
```


```c
#include <JFH.h>

int main() {
    jfh_array_t *elm = JFH_initL();
    jfh_array_t *subelm = JFH_initL();
    if (!elm || !subelm) return 1;

    JFH_setdoubleL(elm, 3.1415);
    JFH_setnullL(JFH_appendL(elm));
    JFH_setarrL(JFH_appendL(elm), subelm);

    JFH_setboolL(subelm, true);
    JFH_setintL(JFH_appendL(subelm), 123);
    JFH_setstrL(JFH_appendL(subelm), "Hello, World!");

    char *json = JFH_indent_json(JFH_encode_arr(elm), 4);
    if (!json) return 1;
    printf("JSON: \n%s\n", json);

    JFH_free_list(elm);
    free(json);
	return 0;
}
```  

Output:  
```json
JSON:  
[  
    3.141500,  
    null,  
    [  
        true,  
        123,  
        "Hello, World!"  
    ]  
]
```


##### Parsing

```c
#include <JFH.h>

int main() {
    char *json = "[3.1415, null, [true, 123, \"Hello, World!\"]]";

    jfh_array_t *arr = JFH_parse_arr(json);
    if (!arr) return 1;

    char *encoded = JFH_indent_json(JFH_encode_arr(arr), 4);
    if (!encoded) return 1;

    printf("JSON: \n%s\n", encoded);

    JFH_free_list(arr);
    free(encoded);
    return 0;
}
```  

Output:  
```json
JSON:  
[  
    3.141500,  
    null,  
    [  
        true,  
        123,  
        "Hello, World!"  
    ]  
]
```


#### Objects & Arrays

Either an object has an array or an array has an object.

```c
#include <JFH.h>

int main() {
    char *json = "{\"JSON-File-Handler\": {\"Examples\": {\"Example1\": [\"CompileMethod.txt\", \"main.c\"],\"Example2\": [\"CompileMethod.txt\", \"main.c\"],\"Example3\": [\"CompileMethod.txt\", \"main.c\",\"Test.json\"]},\"Headers\": [\"data_structures.h\", \"json_format.h\", \"numtostr.h\", \"utils.h\", \"JFH.h\"],\"Source Files\": [\"data_structures.c\", \"json_format.c\", \"numtostr.c\", \"utils.c\"]}}";

    jfh_obj_t *obj = JFH_parse_obj(json);
    if (!obj) return 1;

    char *encoded = JFH_indent_json(JFH_encode_obj(obj), 4);
    if (!encoded) return 1;

    printf("JSON: \n%s\n", encoded);

    JFH_free_map(obj);
    free(encoded);
}
```  

Output:  
```json
JSON:  
{  
    "JSON-File-Handler": {  
        "Examples": {  
            "Example1": [  
                "CompileMethod.txt",  
                "main.c"  
            ],  
            "Example2": [  
                "CompileMethod.txt",  
                "main.c"  
            ],  
            "Example3": [  
                "CompileMethod.txt",  
                "main.c",  
                "Test.json"  
            ]  
        },  
        "Headers": [  
            "data_structures.h",  
            "json_format.h",  
            "numtostr.h",  
            "utils.h",  
            "JFH.h"  
        ],  
        "Source Files": [  
            "data_structures.c",  
            "json_format.c",  
            "numtostr.c",  
            "utils.c"  
        ]  
    }  
}
```


## After v1.0.0

Because I will be doing other things, this library won't be getting as much updates as much as before.
And I will definitely create more features, and possibly the features in [Unsupported features](#Unsupported-features).