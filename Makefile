# Makefile

# Convenience wrapper around CMake commands

# Variables
CMAKE_BUILD_TYPE ?= Release
BUILD_DIR = build

# Targets
.PHONY: all build clean test debug release docs

all: build

build:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) && cmake --build $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

test:
	$(BUILD_DIR)/run_tests

debug:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug && cmake --build $(BUILD_DIR)

release:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release && cmake --build $(BUILD_DIR)

docs:
	@echo "Placeholder for Doxygen documentation generation"