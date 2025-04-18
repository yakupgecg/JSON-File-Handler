#include "..\Headers\parser.h"
#include <stdio.h>
#include <stdlib.h>

char *parseShort(short value) {
    size_t size = snprintf(NULL, 0, "%hd", value);
    char *buffer = malloc(size + 1);
    if (!buffer) return NULL;
    sprintf(buffer, "%hd", value);
    return buffer;
}

char *parseInt(int value) {
    size_t size = snprintf(NULL, 0, "%d", value);
    char *buffer = malloc(size + 1);
    if (!buffer) return NULL;
    sprintf(buffer, "%d", value);
    return buffer;
}

char *parseFloat(float value) {
    size_t size = snprintf(NULL, 0, "%g", value);
    char *buffer = malloc(size + 1);
    if (!buffer) return NULL;
    sprintf(buffer, "%g", value);
    return buffer;
}

char *parseDouble(double value) {
    size_t size = snprintf(NULL, 0, "%lf", value);
    char *buffer = malloc(size + 1);
    if (!buffer) return NULL;
    sprintf(buffer, "%lf", value);
    return buffer;
}

char *parseLong(long value) {
    size_t size = snprintf(NULL, 0, "%ld", value);
    char *buffer = malloc(size + 1);
    if (!buffer) return NULL;
    sprintf(buffer, "%ld", value);
    return buffer;
}

char *parseLl(long long value) {
    size_t size = snprintf(NULL, 0, "%lld", value);
    char *buffer = malloc(size + 1);
    if (!buffer) return NULL;
    sprintf(buffer, "%lld", value);
    return buffer;
}

char *parseLd(long double value) {
    size_t size = snprintf(NULL, 0, "%Lf", value);
    char *buffer = malloc(size + 1);
    if (!buffer) return NULL;
    sprintf(buffer, "%Lf", value);
    return buffer;
}