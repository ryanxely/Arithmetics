#ifndef UTILITIES_H
#define UTILITIES_H

#ifdef __cplusplus
extern "C" {
#endif

char* concat(const char* first, ...);
char* substr(const char* src, int start, int len);
int   contains(const char* word, const char** set, int set_length);

void test_utilities();

#ifdef __cplusplus
}
#endif

#endif