#include "..\Headers\numtostr.h"

char *JFH_str_Int(int value) {
    size_t size = snprintf(NULL, 0, "%d", value);
    char *buffer = malloc(size + 1);
    if (!buffer) {
        errno = ENOMEM;
        return NULL;
    }
    sprintf(buffer, "%d", value);
    return buffer;
}

char *JFH_str_Double(double value) {
    size_t size = snprintf(NULL, 0, "%lf", value);
    char *buffer = malloc(size + 1);
    if (!buffer) {
        errno = ENOMEM;
        return NULL;
    }
    sprintf(buffer, "%lf", value);
    return buffer;
}