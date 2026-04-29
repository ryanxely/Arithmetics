#include <math.h>
#include <stdio.h>
#include <string.h>
#include "core/numeric_methods.h"
#include "core/math_parser.h"

/*
- Numerical derivative of f at x using the central-difference formula:
    f’(x) ≈ (f(x+h) - f(x-h)) / (2h)
*/
#define DERIV_H 1e-5f // Taille du pas pour l'approximation de la dérivée numérique

static float numerical_derivative(char* expr, float x) {
    float fwd = evaluate_expression(expr, x + DERIV_H);
    float bwd = evaluate_expression(expr, x - DERIV_H);
    return (fwd - bwd) / (2.0f * DERIV_H);
}

/*
- Newton-Raphson root finder.
- Returns the root, or NAN if the method did not converge.
*/
float newton_raphson(const char* expr, float x0, float tol, int max_iter) {
    /* We need a mutable copy because evaluate_expression takes char* */
    char buf[512];
    strncpy(buf, expr, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    
    float x = x0;
    printf("On execute la methode the Raphson sur la fonction: %s\n", expr);
    printf("Valeur initiale:\t%.6f\n", x0);
    printf("Tolerance : %.2e,\tIterations maximales :%d\n\n", tol, max_iter);
    
    for (int iter = 0; iter < max_iter; iter++) {
        float fx  = evaluate_expression(buf, x);
        float dfx = numerical_derivative(buf, x);
        
        printf("Iteration %d:\n", iter + 1);
        printf("\tx%d = %.6f\n", iter, x);
        printf("\tf(x%d) = %.6f\n", iter, fx);
        printf("\tf'(x%d) = %.6f\n", iter, dfx);
        
        if (isnan(fx) || isnan(dfx)) {
            fprintf(stderr, "newton_raphson: evaluation returned NaN at x=%.6f\n", x);
            return NAN;
        }
        
        if (fabsf(dfx) < 1e-12f) {
            fprintf(stderr, "newton_raphson: derivative near zero at x=%.6f, aborting\n", x);
            return NAN;
        }
        
        float x_new = x - fx / dfx;
        printf("\tx%d = %.6f\n", iter + 1, x_new);
        printf("\tDx%d = %.2e\n\n", iter + 1, fabsf(x_new - x));
        
        if (fabsf(x_new - x) < tol) {
            printf("Converge apres  %d iterations.\n", iter + 1);
            return x_new;   /* converged */
        }
        
        x = x_new;
    }
    
    fprintf(stderr, "newton_raphson: ne converge pas apres %d iterations\n", max_iter);
    return NAN;
}