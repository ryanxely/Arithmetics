#include <iostream>
#include "core/module_template.h"

int main() {
    // Print startup banner
    std::cout << "Welcome to the Universal C/C++ Project Template!" << std::endl;

    // Read a configuration value (placeholder)
    int configValue = 42; // This would typically come from a config file or user input

    // Call a core function (example)
    int result = coreFunction(configValue);
    std::cout << "Core function result: " << result << std::endl;

    return 0;
}