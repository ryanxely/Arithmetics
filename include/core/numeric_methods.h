#ifndef NUMERIC_METHODS_H
#define NUMERIC_METHODS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Newton-Raphson */

/* One row of the iteration table */
typedef struct {
    int   n;        /* iteration number        */
    float x_n;      /* current x               */
    float fx_n;     /* f(x_n)                  */
    float dfx_n;    /* f'(x_n)                 */
    float x_next;   /* x_(n+1)                 */
    float delta_x;  /* |x_(n+1) - x_n|         */
} NRIteration;

/* Full result — table + metadata */
typedef struct {
    /* Input parameters */
    char  expr[512];
    float x0;
    float tolerance;
    int   max_iter;

    /* Output */
    NRIteration* iterations;  /* dynamically allocated array  */
    int          iter_count;  /* how many rows were filled     */
    float        root;        /* final converged value or NAN  */
    int          converged;   /* 1 = success, 0 = failed       */
} NRResult;

/* Public API */
/**
 * @brief Implementation de la methode de Newton-Raphson pour trouver les racines d'une fonction.
 * 
 * @param expr - Fonction en chaine de caracteres en terme de x  (e.g. "x^3 - 2*x + 1")
 * @param x0 - Valeur initiale
 * @param tol - Tolerance de convergence   (e.g. 1e-6)
 * @param max_iter - Iterations maximales avant d'abandonner
 * @return float 
 */
NRResult newton_raphson(const char* expr, float x0, float tol, int max_iter);
void     nr_print      (const NRResult* r);   /* replaces your printfs  */
void     nr_free       (NRResult* r);         /* frees iterations array */


#ifdef __cplusplus
}
#endif

#endif // NUMERIC_METHODS_H
