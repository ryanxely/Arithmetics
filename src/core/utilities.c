#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utilities.h"


char* concat(const char* first, ...) {
    va_list args;
    size_t len = 0;

    va_start(args, first);
    const char* s = first;

    while (s) {
        len += strlen(s);
        s = va_arg(args, const char*);
    }
    va_end(args);

    char* result = malloc(len + 1);
    result[0] = '\0';

    va_start(args, first);
    s = first;
    while (s) {
        strcat(result, s);
        s = va_arg(args, const char*);
    }
    va_end(args);

    return result;
}

char* substr(const char* src, int start, int len) {
    char* out = malloc(len + 1);
    strncpy(out, src + start, len);
    out[len] = '\0';
    return out;
}

int contains(const char* word, const char** functions, int len) {
    for (int i = 0; i < len; i++) {
        if (strcmp(word, functions[i]) == 0)
            return 1;
    }
    return 0;
}