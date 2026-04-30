#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "core/math_parser.h"
#include "core/numeric_methods.h"
#include "core/utilities.h"

/**/
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Utilisation:\n\t%s <expression> [x=<valeur_initiale>] [tol=<tolerance>] [nmax=<iterations_maximales>]\n", "arithmetics");
        return -1;
    }

    const char* keys[3] = {
        "x", "tol", "nmax"
    };

    char expr[512];
    float x = 1.0f, tol = 1e-6f;
    int nmax = 100;
    
    if(argc >= 2){
        strncpy(expr, argv[1], sizeof(expr)-1);
        expr[sizeof(expr)-1] = '\0';
    
        for(int i = 2; i < argc; i++){
            const char* ptr = strchr(argv[i], '=');
            if (!ptr) {
                printf("Erreur de syntaxe !!\n");
                return -1;
            }

            int j = ptr - argv[i];
            char* key = (char*)malloc(j + 1);
            if (!key) {
                printf("Failed to allocate memory\n");
                return -1;
            }
            strncpy(key, argv[i], j);
            key[j] = '\0';
            
            if (contains(key, keys, sizeof(keys))){
                char val[16];
                strncpy(val, ptr+1, sizeof(val)-1);
                val[sizeof(val)-1] = '\0';
                
                if (strcmp(key, "x") == 0){
                    x = atof(val);
                    if (isnan(x)) {
                        printf("Erreur: Valeur de x incorrecte ! Entrez un nombre\n");
                        return -1;
                    }
                } else if (strcmp(key, "tol") == 0){
                    tol = atof(val);
                    if (isnan(tol) || tol < 0) {
                        printf("Erreur: Valeur de tolerance incorrecte ! Entrez un nombre > 0\n");
                        return -1;
                    }
                } else if (strcmp(key, "nmax") == 0){
                    nmax = atoi(val);
                    // printf("#--->%d\n", i);
                    if (nmax < 2) {
                        printf("Erreur: Valeur de nmax incorrecte ! Entrez un entier naturel > 2\n");
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

    NRResult result = newton_raphson(expr, x, tol, nmax);

    if (isnan(result.root)) {
        printf("Racine non trouvee.\n");
    }

    printf("Fin d\'execution.\n");

    nr_print(&result);

    return 0;
}