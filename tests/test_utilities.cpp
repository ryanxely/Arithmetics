#include <cassert>
#include <iostream>
#include "core/utilities.h"

using namespace std;

void test_utilities() {
    char* st = concat("Hello", " World", " Max", NULL);
    // cout << strlen(st) << endl;
    cout << st << endl;
    free(st);
    
    char* s = concat("My", "Babe", "is", "Crying", NULL);
    char* strip = substr(s, 1, 7);
    cout << strip << endl;
    free(strip);
    free(s);
    
    const char* list[] = {
        "one", "two", "three", "four"
    };
    cout << (contains("three", list, 4) ? "true" : "false");
    cout << endl;
}

int main() {
    test_utilities();
    return 0;
}
