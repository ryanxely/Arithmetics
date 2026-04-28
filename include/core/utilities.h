#ifndef UTILITIES_H
#define UTILITIES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Function description for utilities.
 */

char* concat(const char* first, ...);
char* substr(const char* src, int start, int len);
int contains(const char* word, const char** functions, int len)

#ifdef __cplusplus
}
#endif

#endif // UTILITIES_H
