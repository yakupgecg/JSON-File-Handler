#include "..\Headers\numtostr.h"

char *str_Short(short value) {
    size_t size = snprintf(NULL, 0, "%hd", value);
    char *buffer = malloc(size + 1);
    if (!buffer) {
        errno = ENOMEM;
        return NULL;
    }
    sprintf(buffer, "%hd", value);
    return buffer;
}

char *str_Int(int value) {
    size_t size = snprintf(NULL, 0, "%d", value);
    char *buffer = malloc(size + 1);
    if (!buffer) {
        errno = ENOMEM;
        return NULL;
    }
    sprintf(buffer, "%d", value);
    return buffer;
}

char *str_Float(float value) {
    size_t size = snprintf(NULL, 0, "%g", value);
    char *buffer = malloc(size + 1);
    if (!buffer) {
        errno = ENOMEM;
        return NULL;
    }
    sprintf(buffer, "%g", value);
    return buffer;
}

char *str_Double(double value) {
    size_t size = snprintf(NULL, 0, "%lf", value);
    char *buffer = malloc(size + 1);
    if (!buffer) {
        errno = ENOMEM;
        return NULL;
    }
    sprintf(buffer, "%lf", value);
    return buffer;
}

char *str_Long(long value) {
    size_t size = snprintf(NULL, 0, "%ld", value);
    char *buffer = malloc(size + 1);
    if (!buffer) {
        errno = ENOMEM;
        return NULL;
    }
    sprintf(buffer, "%ld", value);
    return buffer;
}

char *str_Ll(long long value) {
    size_t size = snprintf(NULL, 0, "%lld", value);
    char *buffer = malloc(size + 1);
    if (!buffer) {
        errno = ENOMEM;
        return NULL;
    }
    sprintf(buffer, "%lld", value);
    return buffer;
}

char *str_Ld(long double value) {
    size_t size = snprintf(NULL, 0, "%Lf", value);
    char *buffer = malloc(size + 1);
    if (!buffer) {
        errno = ENOMEM;
        return NULL;
    }
    sprintf(buffer, "%Lf", value);
    return buffer;
}