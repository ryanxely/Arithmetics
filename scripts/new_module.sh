#!/bin/bash

# This script generates a new module in the project.
# Usage: ./new_module.sh <module_name>

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <module_name>"
    exit 1
fi

MODULE_NAME=$1

# Create header file
cat <<EOL > include/core/${MODULE_NAME}.h
#ifndef ${MODULE_NAME^^}_H
#define ${MODULE_NAME^^}_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Function description for ${MODULE_NAME}.
 */
void ${MODULE_NAME}_function();

#ifdef __cplusplus
}
#endif

#endif // ${MODULE_NAME^^}_H
EOL

# Create source file
cat <<EOL > src/core/${MODULE_NAME}.c
#include "${MODULE_NAME}.h"

/**
 * @brief Implementation of ${MODULE_NAME} function.
 */
void ${MODULE_NAME}_function() {
    // TODO: Implement ${MODULE_NAME} logic here.
}
EOL

# Create test file
cat <<EOL > tests/test_${MODULE_NAME}.cpp
#include <cassert>
#include "../include/core/${MODULE_NAME}.h"

void test_${MODULE_NAME}() {
    // TODO: Add tests for ${MODULE_NAME}.
    assert(true); // Placeholder assertion
}

int main() {
    test_${MODULE_NAME}();
    return 0;
}
EOL

echo "Module ${MODULE_NAME} created successfully."