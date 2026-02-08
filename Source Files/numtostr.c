#include "..\Headers\numtostr.h"
#include <inttypes.h>

char *JFH_str_Int(int64_t value) {
    size_t size = snprintf(NULL, 0, "%" PRIi64, value);
    char *buffer = malloc(size + 1);
    if (!buffer) {
        errno = ENOMEM;
        return NULL;
    }
    sprintf(buffer, "%" PRIi64, value);
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