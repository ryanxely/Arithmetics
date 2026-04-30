#include <cassert>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include "../include/core/numeric_methods.h"

using namespace std;

void test_numeric_methods() {
    NRResult result = newton_raphson("x^2 - 2", 1.0f, 1e-6f, 100);

    if (isnan(result.root)) {
        printf("Racine non trouvee.\n");
    }

    // assert(true);

    nr_print(&result);
}

int main() {
    test_numeric_methods();
    return 0;
}
