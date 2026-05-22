#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "core/numeric_integration.h"

static void afficher_resultat(ResultatIntegration res) {
    printf("\nMethode : %s\n", res.methode);
    printf("Intervalle : [%f, %f], n = %d\n", res.a, res.b, res.n);
    printf("Pas h = %f\n", res.h);

    printf("\nPoints et evaluations :\n");
    for (int i = 0; i <= res.n; i++) {
        printf("  x%d = %f   f(x%d) = %f\n", i, res.x[i], i, res.fx[i]);
    }

    printf("\nEtapes de la somme :\n");

    if (strcmp(res.methode, "Trapeze") == 0) {
        printf("  Etape 1 : f(x0) + f(x%d) = %f  (termes aux bornes)\n",  res.n, res.contributions[0] + res.contributions[res.n]);
        for (int i = 1; i < res.n; i++) {
            printf("  Etape %d : += 2*f(x%d) = %f   somme cumulee = %f\n", i + 1, i, res.contributions[i], res.sommes_cumulees[i]);
        }
    } else {
        printf("  Etape 1 : f(x0) + f(x%d) = %f  (termes aux bornes)\n", res.n, res.contributions[0] + res.contributions[res.n]);
        for (int i = 1; i < res.n; i++) {
            int coeff = (i % 2 != 0) ? 4 : 2;
            printf("  Etape %d : += %d*f(x%d) = %f   somme cumulee = %f\n", i + 1, coeff, i, res.contributions[i], res.sommes_cumulees[i]);
        }
    }

    printf("\nSomme totale = %f\n", res.somme_totale);

    if (strcmp(res.methode, "Trapeze") == 0) {
        printf("Resultat : (h/2) * %f = %f\n", res.somme_totale, res.resultat);
    } else {
        printf("Resultat : (h/3) * %f = %f\n", res.somme_totale, res.resultat);
    }
}

int main(int argc, char *argv[]) {
    double a = 0.0, b = 0.0;
    int n = 0;
    char methode[20] = "";
    char expr[512] = "";
    int a_set = 0, b_set = 0, n_set = 0, m_set = 0;

    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "--a") == 0) {
            a = atof(argv[i + 1]);
            a_set = 1;
        } else if (strcmp(argv[i], "--b") == 0) {
            b = atof(argv[i + 1]);
            b_set = 1;
        } else if (strcmp(argv[i], "--n") == 0) {
            n = atoi(argv[i + 1]);
            n_set = 1;
        } else if (strcmp(argv[i], "--methode") == 0) {
            strncpy(methode, argv[i + 1], 19);
            m_set = 1;
        } else if (strcmp(argv[i], "--expr") == 0) {
            strncpy(expr, argv[i + 1], sizeof(expr)-1);
            expr[sizeof(expr)-1] = '\0';
        }
    }

    if (!a_set || !b_set || !n_set || !m_set) {
        fprintf(stderr, "Usage : %s --a <val> --b <val> --n <val> --expr <expr> --methode <trapeze|simpson>\n", argv[0]);
        return 1;
    }

    if (a >= b) {
        fprintf(stderr, "Erreur : a doit etre strictement inferieur a b.\n");
        return 1;
    }

    if (n <= 0) {
        fprintf(stderr, "Erreur : n doit etre un entier strictement positif.\n");
        return 1;
    }

    if (n > MAX_POINTS - 1) {
        fprintf(stderr, "Erreur : n ne doit pas depasser %d.\n", MAX_POINTS - 1);
        return 1;
    }

    if (strcmp(methode, "simpson") == 0 && n % 2 != 0) {
        fprintf(stderr, "Erreur : n doit etre pair pour la methode de Simpson.\n");
        return 1;
    }

    if (strcmp(methode, "trapeze") != 0 && strcmp(methode, "simpson") != 0) {
        fprintf(stderr, "Erreur : methode inconnue. Choisir 'trapeze' ou 'simpson'.\n");
        return 1;
    }

    ResultatIntegration res;

    if (strcmp(methode, "trapeze") == 0) {
        res = trapeze(expr, a, b, n);
    } else {
        res = simpson(expr, a, b, n);
    }

    afficher_resultat(res);

    return 0;
}
