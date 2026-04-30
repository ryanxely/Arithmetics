#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
NRResult newton_raphson(const char* expr, float x0, float tol, int max_iter) {
    NRResult result;

    /* Store input parameters */
    strncpy(result.expr, expr, sizeof(result.expr) - 1);
    result.expr[sizeof(result.expr) - 1] = '\0';
    result.x0        = x0;
    result.tolerance = tol;
    result.max_iter  = max_iter;

    result.converged = 0;
    result.root      = NAN;
    result.iter_count = 0;

    /* Allocate iteration table — worst case max_iter rows */
    result.iterations = (NRIteration*)malloc(sizeof(NRIteration) * max_iter);
    if (!result.iterations) return result;

    char buf[512];
    strncpy(buf, expr, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    float x = x0;
    
    int i = 0;
    while (i < max_iter) {
        float fx  = evaluate_expression(buf, x);
        float dfx = numerical_derivative(buf, x);

        /* Fill this iteration's row */
        NRIteration row;
        row.n     = i;
        row.x_n   = x;
        row.fx_n  = fx;
        row.dfx_n = dfx;

        if (isnan(fx) || isnan(dfx) || fabsf(dfx) < 1e-12f) {
            row.x_next  = NAN;
            row.delta_x = NAN;
            result.iterations[result.iter_count++] = row;
            return result;   /* converged = 0, root = NAN */
        }

        float x_new   = x - fx / dfx;
        row.x_next    = x_new;
        row.delta_x   = fabsf(x_new - x);

        result.iterations[result.iter_count++] = row;

        if (row.delta_x < tol) {
            result.converged = 1;
            result.root      = x_new;
            return result;
        }

        x = x_new;

        i++;
    }

    if (i == max_iter){
        while(--i > 0){
            /* We recursively test for absolute convergence */
            if (result.iterations[i].delta_x - result.iterations[i-1].delta_x > 0) return result;
        }

        /* Converges */
        result.converged = 1;
        result.root      = result.iterations[result.iter_count - 1].x_next;
    }

    return result;   /* max iterations reached, converged = 0 */
}

void nr_print(const NRResult* r) {
    printf("%-70s\n", "--------------------------------------------------------------------------");
    printf("%-70s\n", "                        Methode de Newton Raphson");
    printf("%-70s\n", "--------------------------------------------------------------------------");
    printf("f(x) = %s\t\tx0 = %.6f\n", r->expr, r->x0);
    printf("Tolerance = %.2e\tIterations Maximales = %d\n\n", r->tolerance, r->max_iter);

    printf("%-6s| %-12s| %-12s| %-12s| %-12s| %-12s\n",
        "n", "x_n", "f(x_n)", "f'(x_n)", "x_(n+1)", "delta_x");
    printf("%-70s\n", "--------------------------------------------------------------------------");

    for (int i = 0; i < r->iter_count; i++) {
        NRIteration* row = &r->iterations[i];
        printf("%-6d| %-12.6f| %-12.6f| %-12.6f| %-12.6f| %-12.2e\n",
            row->n, row->x_n, row->fx_n, row->dfx_n,
            row->x_next, row->delta_x);
    }

    printf("\n");
    if (r->converged)
        printf("Converge apres %d iterations ->  racine ~ %.7f\n",
            r->iter_count, r->root);
    else
        printf("Ne converge pas apres %d iterations.\n", r->iter_count);
}

void nr_free(NRResult* r) {
    free(r->iterations);
    r->iterations = NULL;
    r->iter_count = 0;
}