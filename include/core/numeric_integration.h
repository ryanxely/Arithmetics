#ifndef NUMERIC_INTEGRATION_H
#define NUMERIC_INTEGRATION_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Function description for numeric_integration.
 */

#define MAX_POINTS 10000

typedef struct {
    char methode[20];
    double a;
    double b;
    int n;
    double h;
    double x[MAX_POINTS];
    double fx[MAX_POINTS];
    double contributions[MAX_POINTS];
    double sommes_cumulees[MAX_POINTS];
    double somme_totale;
    double resultat;
} ResultatIntegration;

ResultatIntegration trapeze(char *expr, double a, double b, int n);
ResultatIntegration simpson(char *expr, double a, double b, int n);


#ifdef __cplusplus
}
#endif

#endif // NUMERIC_INTEGRATION_H
