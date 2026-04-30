#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "core/math_parser.h"
#include "core/numeric_methods.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Utilisation: %s <expression> [x=<valeur_initiale>] [tol=<tolerance>] [nmax=<iterations_maximales>]\n", argv[0]);
        return -1;
    }

    const char* keys[3] = {
        "x", "tol", "nmax"
    };

    char expr_buffer[512];
    float x = 1.0f, tol = 1e-6f;
    int nmax = 100;
    
    if(argc == 2){
        strncpy(expr_buffer, argv[1], sizeof(expr_buffer)-1);
        expr_buffer[sizeof(expr_buffer)-1] = '\0';
    } else {
        for(int i = 2; i < argc, i++){
            const char* ptr = strchr(argv[i], "=");
            if (!ptr) {
                printf("Erreur de syntaxe !\n");
                return -1;
            }

            const char* key = (char*)malloc(ptr + 1);
            if (!key) return NULL;
            strncpy(key, argv[0], ptr);
            key[ptr] = '\0';
            
            if (contains(key, keys)){
                char val[16];
                strcpy(val, ptr+1, sizeof(val)-1);
                val[sizeof(val)-1] = '\0';

                if (strcmp(key, "x") == 0){
                    x = atof(val);
                    if (isnan(x)) {
                        printf("Erreur: Valeur de x incorrecte !\n");
                        return -1;
                    }
                } else if (strcmp(key, "tol") == 0){
                    tol = atof(val);
                    if (isnan(x)) {
                        printf("Erreur: Valeur de tolerance incorrecte !\n");
                        return -1;
                    }
                } else if (strcmp(key, "nmax") == 0){
                    nmax = atoi(val);
                    if (isnan(x)) {
                        printf("Erreur: Valeur de nmax incorrecte !\n");
                        return -1;
                    }
                } else {
                    printf("Parametre inconnu: %s !\n", key);
                    return -1;
                }
            }

        }
    }

    // const char* expr = argv[1];

    NRResult result = newton_raphson(expr, x0, tol, max_iter);

    if (!result) {
        printf("Racine non trouvee.\n");
    }

    print_nr(result);

    return 0;
}