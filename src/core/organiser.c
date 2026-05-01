#include "organiser.h"

/**
 * @brief Implementation of organiser function.
 */


typedef enum {
    NUMERIC_METHODS
} MODULE;

#define N_MODULES 1

const char* getModuleNameFr(MODULE m){
    switch(m){
        case NUMERIC_METHODS: return "Methodes Numeriques";
        default: return "Non defini";
    }
}
const char* getModuleNameEn(MODULE m){
    switch(m){
        case NUMERIC_METHODS: return "Numeric Methods";
        default: return "Undefined";
    }
}