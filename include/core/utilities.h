#ifndef UTILITIES_H
#define UTILITIES_H

#ifdef __cplusplus
extern "C" {
#endif

char* concat(const char* first, ...);
char* substr(const char* src, int start, int len);
int   contains(const char* word, const char** functions, int len);

void test_utilities();

#ifdef __cplusplus
}
#endif

#endif