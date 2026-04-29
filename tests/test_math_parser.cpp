#include <cassert>
#include <iostream>
#include "../include/core/math_parser.h"

using namespace std;

void test_math_parser() {
    cout << evaluate_expression("sin(90)", 2) << endl;
}

int main() {
    test_math_parser();
    return 0;
}


