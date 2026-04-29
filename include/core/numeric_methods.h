#ifndef NUMERIC_METHODS_H
#define NUMERIC_METHODS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Implementation de la methode de Newton-Raphson pour trouver les racines d'une fonction.
 * 
 * @param expr - Fonction en chaine de caracteres en terme de x  (e.g. "x^3 - 2*x + 1")
 * @param x0 - Valeur initiale
 * @param tol - Tolerance de convergence   (e.g. 1e-6)
 * @param max_iter - Iterations maximales avant d'abandonner
 * @return float 
 */
float newton_raphson(const char* expr, float x0, float tol, int max_iter);

#ifdef __cplusplus
}
#endif

#endif // NUMERIC_METHODS_H
