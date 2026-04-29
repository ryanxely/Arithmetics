#include <cassert>
#include "../include/core/numeric_methods.h"

void test_numeric_methods() {
    newton_raphson("x^2 - 2", 1.0f, 1e-6f, 100); // Should converge to sqrt(2) ~ 1.414213
}

int main() {
    test_numeric_methods();
    return 0;
}
