#include <JFH.h>

/*
This example shows a real world usage for JFH. In this example you can imagine there is an app, and some of the apps properties are stored
in a json file. JFH parses that json and puts the data on structs. Sometimes when a real app is being built, properties can be put onto json
files for easier maintainability.
*/

typedef struct {
    int size_x;
    int size_y;
    int pos_x;
    int pos_y;
    bool visible;
    char *theme;
} window;

typedef struct {
    char *default_font;
    char *logo;
    char *supported_os;
    bool confirm_close;
} sys_config;

typedef struct {
    size_t volume;
    bool muted;
} audio;

typedef struct {
    char *name;
    bool archived;
    jfh_obj_t *data;
    jfh_array_t *mods;
} save;

typedef struct {
    window w;
    sys_config conf;
    audio aud;
	save *saves;
	size_t save_count;
} app;

int main() {
    FILE *file = fopen("settings.json", "r");
	if (!file) return 1;
    char buf[2048];
    size_t l = fread(buf, sizeof(char), 2048, file);
	buf[l] = '\0';
    jfh_obj_t *obj = JFH_parse_obj(buf);
    if (!obj) {
		fclose(file);
        return 1;
    }
	
    jfh_obj_t *ewindow = JFH_searchH(obj, "Window");
    jfh_obj_t *esystem_configurations = JFH_searchH(obj, "System_configurations");
    jfh_obj_t *eaudio = JFH_searchH(obj, "Audio");
    jfh_array_t *esaves = JFH_searchH(obj, "Saves")->value.value.arr;
    if (!ewindow || !esystem_configurations || !eaudio || !esaves) {
		fclose(file);
		JFH_free_map(obj);
		return 1;
	}
    app a;
	
    a.w.size_x = JFH_getelementbyindex(JFH_searchH(ewindow, "Size")->value.value.arr, 0)->value.value.i;
    a.w.size_y = JFH_getelementbyindex(JFH_searchH(ewindow, "Size")->value.value.arr, 1)->value.value.i;
    a.w.pos_x = JFH_getelementbyindex(JFH_searchH(ewindow, "Position")->value.value.arr, 0)->value.value.i;
    a.w.pos_y = JFH_getelementbyindex(JFH_searchH(ewindow, "Position")->value.value.arr, 1)->value.value.i;
    a.w.visible = JFH_searchH(ewindow, "Visible")->value.value.b;
    a.w.theme = JFH_searchH(ewindow, "Theme")->value.value.str.str;
	
	a.conf.default_font = JFH_searchH(esystem_configurations, "Default font")->value.value.str.str;
	a.conf.logo = JFH_searchH(esystem_configurations, "Logo")->value.value.str.str;
	a.conf.supported_os = JFH_searchH(esystem_configurations, "Supported_os")->value.value.str.str;
	a.conf.confirm_close = JFH_searchH(esystem_configurations, "Confirm_close")->value.value.b;
	
	a.aud.volume = JFH_searchH(eaudio, "Volume")->value.value.i;
	a.aud.muted = JFH_searchH(eaudio, "Muted")->value.value.b;
	
	a.save_count = JFH_list_len(esaves);
	a.saves = malloc(a.save_count * sizeof(save));
	if (!a.saves) {
		fclose(file);
		JFH_free_map(obj);
		return 1;
	}
	save *s = a.saves;
	
	jfh_array_t *cur = esaves;
	while (cur) {
		s->name = JFH_searchL(cur, "Name")->value.value.str.str;
		s->archived = JFH_searchL(cur, "Archived")->value.value.b;
		s->data = JFH_searchL(cur, "Data")->value.value.obj;
		s->mods = JFH_searchL(cur, "Mods")->value.value.arr;
		if (!s->name || !s->data || !s->mods) {
			JFH_free_map(obj);
    		fclose(file);
			free(a.saves);
    		return 1;
		}
		cur = cur->next;
		s++;
	}

    JFH_free_map(obj);
    fclose(file);
	free(a.saves);
    return 0;
}

/*
There is no expected output. This is just a demo of how to use JFH on more meaningful scenarios.
*/