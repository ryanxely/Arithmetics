/**
 * @file numeric_methods_main.c
 * @author your name (you@domain.com)
 * @brief Main entry point for the numerical methods CLI
 * @version 0.1
 * @date 2026-05-01
 * 
 * @details {
    * description: This file serves as the main entry point for the numerical methods command-line interface (CLI). It provides a modular approach to executing various numerical methods, starting with the Newton-Raphson method. Users can invoke specific methods directly from the CLI by providing the necessary parameters and options.
    * deliverables: build/cli/numeric_methods.exe
    * included in the release of the final Arithmetics app. 
    }
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "core/math_parser.h"
#include "core/numeric_methods.h"
#include "core/utilities.h"

/*  Modular approach
    We will make release for different modules hence users will get many executables to run on cli
    Instead of using cli menus, we enable users to get advantage of the cli cache memory
    Hence will just type the command and set parameters with a help guide */

static void indent(int n){
    while(n--){
        printf("   ");
    }
}

static void help(const char* module, int indent_level){
    if (strcmp(module, "newton-raphson") == 0){
        printf("\n");
        indent(indent_level);
        printf("%s <expression> [x=<valeur_initiale>] [tol=<tolerance>] [nmax=<iterations_maximales>]\n", module);
        printf("\n");
        indent_level++;
        indent(indent_level);
        printf("OPTIONS\n");
        indent(indent_level+1);
        printf("x=<valeur_initiale>\t: Valeur initiale pour la methode de Newton-Raphson (defaut: 1.0)\n");
        indent(indent_level+1);
        printf("tol=<tolerance>\t: Tolerance pour la methode de Newton-Raphson (defaut: 1e-6)\n");
        indent(indent_level+1);
        printf("nmax=<iterations_maximales>\t: Nombre maximum d'iterations pour la methode de Newton-Raphson (defaut: 100)\n");
        printf("\n");
        indent(indent_level);
        printf("EXAMPLE: %s \"x^2 - 2\" x=1.0 tol=1e-6 nmax=100\n", module);
    } else {
        printf("How to use it:\n");
        printf("\n");
        indent(indent_level+1);
        printf("%s <methode> <<options>>\n", module);
        printf("\n");
        indent(indent_level);
        printf("METHODES\n");
        help("newton-raphson", indent_level+1);
    }
}

static int newton_raphson_index(int argc, char* argv[]){
    if (argc < 2) {
        help(argv[0], 0);
        return -1;
    }

    const char* keys[] = {
        "x", "tol", "nmax"
    };
    const int nkeys = sizeof(keys) / sizeof(keys[0]);

    char expr[512];
    float x = 1.0f, tol = 1e-6f;
    int nmax = 100;

    if (argc >= 2){
        if (strncmp(argv[1], "x=", 2) == 0 || strncmp(argv[1], "tol=", 4) == 0 || strncmp(argv[1], "nmax=", 5) == 0) {
            printf("Erreur: Expression manquante.\n");
            help("newton-raphson", 1);
            return -1;
        }

        strncpy(expr, argv[1], sizeof(expr)-1);
        expr[sizeof(expr)-1] = '\0';

        for (int i = 2; i < argc; i++){
            const char* ptr = strchr(argv[i], '=');
            if (!ptr) {
                printf("Numeric - Methods - Erreur de syntaxe: %s\n", argv[i]);
                help("newton-raphson", 1);
                return -1;
            }

            int j = ptr - argv[i];
            if (j <= 0 || j >= 32) {
                printf("Numeric - Methods - Erreur de syntaxe: %s\n", argv[i]);
                help("newton-raphson", 1);
                return -1;
            }

            char key[32];
            strncpy(key, argv[i], j);
            key[j] = '\0';

            if (!contains(key, keys, nkeys)) {
                printf("Parametre inconnu: %s\n", key);
                help("newton-raphson", 1);
                return -1;
            }

            char val[64];
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
                if (isnan(tol) || tol <= 0) {
                    printf("Erreur: Valeur de tolerance incorrecte ! Entrez un nombre > 0\n");
                    return -1;
                }
            } else if (strcmp(key, "nmax") == 0){
                nmax = atoi(val);
                if (nmax < 2) {
                    printf("Erreur: Valeur de nmax incorrecte ! Entrez un entier naturel > 2\n");
                    return -1;
                }
            }
        }
    }

    NRResult result = newton_raphson(expr, x, tol, nmax);

    if (isnan(result.root)) {
        printf("Racine non trouvee.\n");
    }

    nr_print(&result);

    return 0;
}

static int numeric_methods_index(int argc, char* argv[]){
    if (argc < 2) {
        printf("\n");
        help("numeric_methods", 0);
        return -1;
    }

    const char* words[] = {
        "newton-raphson", "help", "-h"
    };
    const int nwords = sizeof(words) / sizeof(words[0]);

    char word[32];
    
    if(argc >= 2){
        strncpy(word, argv[1], sizeof(word)-1);
        word[sizeof(word)-1] = '\0';
    
        if (contains(word, words, nwords)){
            if (strcmp(word, "newton-raphson") == 0){
                argc--;
                argv++;
                return newton_raphson_index(argc, argv);
            } else if (strcmp(word, "help") == 0 || strcmp(word, "-h") == 0){
                help("numeric_methods", 0);
                return 0;
            }
        } else {
            printf("Methode inconnue: %s\n", word);
            help("numeric_methods", 0);
            return -1;
        }
    }

    return 0;
}

int main(int argc, char* argv[]) {
    return numeric_methods_index(argc, argv);
}