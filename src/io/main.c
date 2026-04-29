#include <stdio.h>
#include <math.h>
#include "core/math_parser.h"
#include "core/newton_raphson.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <expression>\n", argv[0]);
        return 1;
    }

    const char* expr = argv[1];
    float x0 = 1.0f; // Valeur initiale
    float tol = 1e-6f;
    int max_iter = 100;

    float root = newton_raphson(expr, x0, tol, max_iter);

    if (isnan(root)) {
        printf("Failed to find root.\n");
    }

    return 0;
}