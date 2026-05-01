#ifndef ORGANISER_H
#define ORGANISER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NUMERIC_METHODS
} MODULE;

const char* getModuleNameFr(MODULE m);
const char* getModuleNameEn(MODULE m);

#ifdef __cplusplus
}
#endif

#endif // ORGANISER_H
