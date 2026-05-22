#include <math.h>
#include <string.h>
#include "core/numeric_integration.h"
#include "core/math_parser.h"

ResultatIntegration trapeze(char *expr, double a, double b, int n) {
    ResultatIntegration res;
    strcpy(res.methode, "Trapeze");
    res.a = a;
    res.b = b;
    res.n = n;
    res.h = (b - a) / n;

    for (int i = 0; i <= n; i++) {
        res.x[i] = a + i * res.h;
        res.fx[i] = evaluate_expression(expr, res.x[i]);
    }

    res.somme_totale = 0.0;

    res.contributions[0] = res.fx[0];
    res.somme_totale += res.contributions[0];
    res.sommes_cumulees[0] = res.somme_totale;

    for (int i = 1; i < n; i++) {
        res.contributions[i] = 2.0 * res.fx[i];
        res.somme_totale += res.contributions[i];
        res.sommes_cumulees[i] = res.somme_totale;
    }

    res.contributions[n] = res.fx[n];
    res.somme_totale += res.contributions[n];
    res.sommes_cumulees[n] = res.somme_totale;

    res.resultat = (res.h / 2.0) * res.somme_totale;

    return res;
}

ResultatIntegration simpson(char *expr,double a, double b, int n) {
    ResultatIntegration res;
    strcpy(res.methode, "Simpson");
    res.a = a;
    res.b = b;
    res.n = n;
    res.h = (b - a) / n;

    for (int i = 0; i <= n; i++) {
        res.x[i] = a + i * res.h;
        res.fx[i] = evaluate_expression(expr, res.x[i]);
    }

    res.somme_totale = 0.0;

    res.contributions[0] = res.fx[0];
    res.somme_totale += res.contributions[0];
    res.sommes_cumulees[0] = res.somme_totale;

    for (int i = 1; i < n; i++) {
        if (i % 2 != 0) {
            res.contributions[i] = 4.0 * res.fx[i];
        } else {
            res.contributions[i] = 2.0 * res.fx[i];
        }
        res.somme_totale += res.contributions[i];
        res.sommes_cumulees[i] = res.somme_totale;
    }

    res.contributions[n] = res.fx[n];
    res.somme_totale += res.contributions[n];
    res.sommes_cumulees[n] = res.somme_totale;

    res.resultat = (res.h / 3.0) * res.somme_totale;

    return res;
}
