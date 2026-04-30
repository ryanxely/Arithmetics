# ─────────────────────────────────────────────
#  ARITHMETICS — Master Makefile
#  Usage: mingw32-make [target]
# ─────────────────────────────────────────────

$(shell mkdir build\cli build\lib build\debug build\release build\tests build\gui 2>nul)

CC       = gcc
CXX      = g++
CFLAGS   = -Wall -Wextra -std=c11   -Iinclude -lm
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -lm

CORE = src/core/numeric_methods.c \
        src/core/math_parser.c    \
        src/core/utilities.c

# Create output folders (runs silently on every make call)
$(shell mkdir build\cli build\lib build\debug build\release build\tests build\gui 2>nul)

# ─────────────────────────────────────────────
#  BUILD TARGETS
# ─────────────────────────────────────────────

# Default — only build what is currently ready
all: cli

cli:
	$(CC) $(CORE) src/io/main.c $(CFLAGS) -o build/cli/arithmetics.exe

cli-cpp:
	$(CXX) $(CORE) src/io/main.cpp $(CXXFLAGS) -o build/cli/arithmetics.exe

debug:
	$(CC) $(CORE) src/io/main.c $(CFLAGS) -g -O0 -o build/debug/arithmetics.exe

release:
	$(CC) $(CORE) src/io/main.c $(CFLAGS) -O2 -o build/release/arithmetics.exe

# ─────────────────────────────────────────────
#  LIBRARY TARGETS
# ─────────────────────────────────────────────

dll:
	$(CC) $(CORE) $(CFLAGS) -shared -fPIC -o build/lib/arithmetics.dll

wasm:
	emcc $(CORE) -Iinclude \
        -s EXPORTED_FUNCTIONS='["_evaluate_expression","_numeric_methods"]' \
        -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
        -o build/lib/arithmetics.js

lib:
	$(CC) $(CFLAGS) -c src/core/numeric_methods.c -o build/lib/numeric_methods.o
	$(CC) $(CFLAGS) -c src/core/math_parser.c    -o build/lib/math_parser.o
	$(CC) $(CFLAGS) -c src/core/utilities.c      -o build/lib/utilities.o
	ar rcs build/lib/arithmetics.a build/lib/numeric_methods.o \
        build/lib/math_parser.o    \
        build/lib/utilities.o

gui:
	$(CXX) $(CORE) src/ui/app.cpp \
        libs/imgui/imgui.cpp \
        libs/imgui/backends/imgui_impl_sdl2.cpp \
        $(CXXFLAGS) -Ilibs/imgui -lSDL2 -lopengl32 \
        -o build/gui/arithmetics.exe

# ─────────────────────────────────────────────
#  TEST TARGETS
# ─────────────────────────────────────────────

test:
	$(CXX) tests/test_$(module).cpp $(CORE) $(CXXFLAGS) -o build/tests/test_$(module).exe
	./build/tests/test_$(module).exe

test-all:
	$(CXX) tests/test_numeric_methods.cpp $(CORE) $(CXXFLAGS) -o build/tests/test_newton.exe
	./build/tests/test_newton.exe
	$(CXX) tests/test_math_parser.cpp    $(CORE) $(CXXFLAGS) -o build/tests/test_parser.exe
	./build/tests/test_parser.exe
	$(CXX) tests/test_utilities.cpp      $(CORE) $(CXXFLAGS) -o build/tests/test_util.exe
	./build/tests/test_util.exe
	@echo "==============================="
	@echo " All tests completed"
	@echo "==============================="

# ─────────────────────────────────────────────
#  UTILITY TARGETS
# ─────────────────────────────────────────────

new-module:
	git-bash ./scripts/new_module.sh $(name)

clean:
	del /Q build\cli\*     2>nul || true
	del /Q build\lib\*     2>nul || true
	del /Q build\debug\*   2>nul || true
	del /Q build\release\* 2>nul || true
	del /Q build\tests\*   2>nul || true
	del /Q build\gui\*     2>nul || true

help:
	@echo.
	@echo  ARITHMETICS — available commands
	@echo  ────────────────────────────────────────────────────────
	@echo  mingw32-make all              builds default (cli only for now)
	@echo  mingw32-make cli              builds CLI exe (C entry point)
	@echo  mingw32-make cli-cpp          builds CLI exe (C++ entry point)
	@echo  mingw32-make debug            builds with debug symbols
	@echo  mingw32-make release          builds optimized
	@echo  mingw32-make dll              builds shared library (.dll)
	@echo  mingw32-make lib              builds static library (.a)
	@echo  mingw32-make wasm             builds WebAssembly (needs Emscripten)
	@echo  mingw32-make gui              builds GUI app (needs ImGui + SDL2)
	@echo  mingw32-make test module=x      tests one module (e.g. name=numeric_methods)
	@echo  mingw32-make test-all         runs all tests
	@echo  mingw32-make new-module name=x creates a new module
	@echo  mingw32-make clean            removes all compiled output
	@echo.