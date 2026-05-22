# -----------------------------------------------------------------
#  ARITHMETICS — Master Makefile
#  Usage: mingw32-make [target]
#
#  First time setup:
#    mingw32-make init        — downloads and installs all libraries
#
#  Daily workflow:
#    mingw32-make cli         — build CLI for one module  (module=name)
#    mingw32-make gui         — build GUI application
#    mingw32-make test        — test one module           (module=name)
#    mingw32-make test-all    — run all tests
# -----------------------------------------------------------------

CC       = gcc
CXX      = g++
CFLAGS   = -Wall -Wextra -std=c11   -Iinclude -lm
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -lm

# Add new module source files here when you create a new module
CORE = 	src/core/numeric_methods.c \
		src/core/math_parser.c     \
		src/core/utilities.c

# -----------------------------------------------------------------
#  LIBRARY CONFIGURATION
# -----------------------------------------------------------------

IMGUI_DIR = libs/imgui
IMGUI_SRC = $(IMGUI_DIR)/imgui.cpp                          \
			$(IMGUI_DIR)/imgui_draw.cpp                     \
			$(IMGUI_DIR)/imgui_tables.cpp                   \
			$(IMGUI_DIR)/imgui_widgets.cpp                  \
			$(IMGUI_DIR)/imgui_demo.cpp                     \
			$(IMGUI_DIR)/backends/imgui_impl_sdl2.cpp       \
			$(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

SDL2_DIR   = libs/SDL2

# ImGui include paths
GUI_FLAGS = -Ilibs/imgui                  \
			-Ilibs/imgui/backends         \
			-Iinclude/ui         		  \
			-Iinclude/ui/SDL2

# Libraries to link for GUI build
GUI_LIBS = 	-L$(SDL2_DIR)/lib            \
			-lSDL2 -lSDL2main            \
			-lopengl32 -lgdi32           \
			-DSDL_MAIN_HANDLED

# -----------------------------------------------------------------
#  FOLDER CREATION  (silent, runs on every make call)
# -----------------------------------------------------------------

$(shell mkdir build/cli 2>nul)
$(shell mkdir build/cli/seperate-files 2>nul)
$(shell mkdir build/lib 2>nul)
$(shell mkdir build/dll 2>nul)
$(shell mkdir build/debug 2>nul)
$(shell mkdir build/release 2>nul)
$(shell mkdir build/tests 2>nul)
$(shell mkdir build/gui 2>nul)
$(shell mkdir libs 2>nul)

# -----------------------------------------------------------------
#  DEFAULT TARGET
# -----------------------------------------------------------------

all: cli-aio

# -----------------------------------------------------------------
#  CLI TARGETS
# -----------------------------------------------------------------

# Build CLI for a specific module:  mingw32-make cli module=numeric_methods
cli:
	$(CXX) $(CORE) src/io/$(module)_io.c $(CXXFLAGS) \
	-o build/cli/seperate-files/$(module).exe

# Build all-in-one CLI with interactive module selection menu
cli-aio:
	$(CXX) $(CORE) src/io/main_index.cpp $(CXXFLAGS) \
	-o build/cli/arithmetics.exe

# -----------------------------------------------------------------
#  DEBUG TARGETS
# -----------------------------------------------------------------

debug:
	$(CXX) $(CORE) src/io/$(module)_io.c $(CXXFLAGS) \
	-g -O0 -o build/debug/seperate-files/$(module).exe

debug-aio:
	$(CXX) $(CORE) src/io/main_index.cpp $(CXXFLAGS) \
	-g -O0 -o build/debug/arithmetics.exe

# -----------------------------------------------------------------
#  RELEASE TARGETS
# -----------------------------------------------------------------

release:
	$(CXX) $(CORE) src/io/$(module)_io.c $(CXXFLAGS) \
	-O2 -o build/release/seperate-files/$(module).exe

release-aio:
	$(CXX) $(CORE) src/io/main_index.cpp $(CXXFLAGS) \
	-O2 -o build/release/arithmetics.exe

# -----------------------------------------------------------------
#  LIBRARY TARGETS
# -----------------------------------------------------------------

# Shared library (.dll) — callable from Python / web server
dll:
	$(CC) $(CORE) $(CFLAGS) -shared -fPIC \
	-o build/dll/arithmetics.dll

# Static library (.a) — linkable into other C/C++ projects
lib:
	$(CC) $(CFLAGS) -c src/core/numeric_methods.c \
	-o build/lib/numeric_methods.o
	$(CC) $(CFLAGS) -c src/core/math_parser.c     \
	-o build/lib/math_parser.o
	$(CC) $(CFLAGS) -c src/core/utilities.c       \
	-o build/lib/utilities.o
	ar rcs build/lib/arithmetics.a            \
	build/lib/numeric_methods.o               \
	build/lib/math_parser.o                   \
	build/lib/utilities.o

# WebAssembly — requires Emscripten installed separately
wasm:
	emcc $(CORE) -Iinclude                                          \
		-s EXPORTED_FUNCTIONS='["_evaluate_expression","_newton_raphson"]' \
		-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]'            \
		-o build/lib/arithmetics.js

# -----------------------------------------------------------------
#  GUI TARGET
# -----------------------------------------------------------------

gui:
	$(CXX) $(CORE) src/ui/main.cpp $(IMGUI_SRC) \
	$(CXXFLAGS) $(GUI_FLAGS) $(GUI_LIBS)               \
	-o build/gui/arithmetics.exe
	@copy libs\SDL2\bin\SDL2.dll build\gui\SDL2.dll >nul 2>&1 || \
	echo "Warning: SDL2.dll not copied - copy manually to build/gui/"

# -----------------------------------------------------------------
#  TEST TARGETS
# -----------------------------------------------------------------

# Test a single module:  mingw32-make test module=numeric_methods
test:
	$(CXX) tests/test_$(module).cpp $(CORE) $(CXXFLAGS) \
	-o build/tests/test_$(module).exe
	./build/tests/test_$(module).exe

# Run all tests in sequence
test-all:
	$(CXX) tests/test_numeric_methods.cpp $(CORE) $(CXXFLAGS) \
	-o build/tests/test_numeric_methods.exe
	./build/tests/test_numeric_methods.exe
	$(CXX) tests/test_math_parser.cpp $(CORE) $(CXXFLAGS)     \
	-o build/tests/test_math_parser.exe
	./build/tests/test_math_parser.exe
	$(CXX) tests/test_utilities.cpp $(CORE) $(CXXFLAGS)       \
	-o build/tests/test_utilities.exe
	./build/tests/test_utilities.exe
	@echo "==============================="
	@echo " All tests passed."
	@echo "==============================="

# -----------------------------------------------------------------
#  UTILITY TARGETS
# -----------------------------------------------------------------

# Create a new module skeleton:  mingw32-make new-module name=gcd
new-module:
	bash scripts/new_module.sh $(name)

# Remove all compiled output (keeps source untouched)
clean:
	del /Q build\cli\*.exe          2>nul || true
	del /Q build\cli\seperate-files\*.exe 2>nul || true
	del /Q build\lib\*              2>nul || true
	del /Q build\dll\*              2>nul || true
	del /Q build\debug\*            2>nul || true
	del /Q build\release\*          2>nul || true
	del /Q build\tests\*.exe        2>nul || true
	del /Q build\gui\arithmetics.exe 2>nul || true

# -----------------------------------------------------------------
#  INIT TARGET  — first-time environment setup
#  Run once after cloning:  mingw32-make init
#
#  What it does:
#    1. Initialises the ImGui git submodule (libs/imgui/)
#    2. Downloads SDL2 MinGW development package
#    3. Extracts headers, .lib files, and SDL2.dll into libs/SDL2/
#
#  Requirements before running init:
#    - MinGW (gcc/g++) must be installed and on PATH
#    - Git must be installed and on PATH
#    - curl must be available (bundled with Windows 10+ and Git Bash)
# -----------------------------------------------------------------

SDL2_VERSION = 2.30.3
SDL2_URL     = https://github.com/libsdl-org/SDL/releases/download/release-$(SDL2_VERSION)/SDL2-devel-$(SDL2_VERSION)-mingw.zip
SDL2_ZIP     = libs/SDL2-devel-$(SDL2_VERSION)-mingw.zip
SDL2_INNER   = SDL2-$(SDL2_VERSION)/x86_64-w64-mingw32

init:
	@echo.
	@echo  -------------------------------------------------------
	@echo  ARITHMETICS — Initialisation de l'environnement
	@echo  -------------------------------------------------------
	@echo.

	@echo  [1/3] Initialisation du sous-module ImGui...
	git submodule update --init --recursive
	@echo  OK - libs/imgui pret.
	@echo.

	@echo  [2/3] Telechargement de SDL2 $(SDL2_VERSION)...
	$(shell mkdir libs\SDL2 2>nul)
	$(shell mkdir include\ui\SDL2 2>nul)
	$(shell mkdir libs\SDL2\bin 2>nul)
	curl -L "$(SDL2_URL)" -o "$(SDL2_ZIP)"
	@echo  OK - archive telechargee.
	@echo.

	@echo  [3/3] Extraction de SDL2...
	tar -xf "$(SDL2_ZIP)" -C libs/ \
			"$(SDL2_INNER)/include/SDL2" \
			"$(SDL2_INNER)/lib"          \
			"$(SDL2_INNER)/bin/SDL2.dll"
	xcopy /E /I /Y "libs\$(SDL2_INNER)\include\SDL2"  "include\ui\SDL2\"       >nul
	xcopy /E /I /Y "libs\$(SDL2_INNER)\lib"           "libs\SDL2\"	           >nul
	copy           "libs\$(SDL2_INNER)\bin\SDL2.dll"  "libs\SDL2\bin\SDL2.dll" >nul
	copy           "libs\SDL2\bin\SDL2.dll"           "build\gui\SDL2.dll"     >nul 2>&1 || true
	del /Q "$(SDL2_ZIP)" 2>nul || true
	@echo  OK - SDL2 installe dans libs/SDL2/ & include/ui/SDL2.
	@echo.
	@echo  -------------------------------------------------------
	@echo  Initialisation terminee. Vous pouvez maintenant builder:
	@echo    mingw32-make gui
	@echo  -------------------------------------------------------
	@echo.

# -----------------------------------------------------------------
#  HELP
# -----------------------------------------------------------------

help:
	@echo.
	@echo  ARITHMETICS — commandes disponibles
	@echo  -------------------------------------------------------
	@echo  mingw32-make init                  premier lancement : installe ImGui et SDL2
	@echo  -------------------------------------------------------
	@echo  mingw32-make all                   build par defaut (cli-aio)
	@echo  mingw32-make cli-aio               CLI interactif tout-en-un
	@echo  mingw32-make cli     module=x      CLI pour un module specifique
	@echo  mingw32-make debug   module=x      build debug d'un module
	@echo  mingw32-make release module=x      build optimise d'un module
	@echo  mingw32-make dll                   bibliotheque partagee (.dll)
	@echo  mingw32-make lib                   bibliotheque statique  (.a)
	@echo  mingw32-make wasm                  WebAssembly (necessite Emscripten)
	@echo  mingw32-make gui                   application graphique (necessite ImGui + SDL2)
	@echo  -------------------------------------------------------
	@echo  mingw32-make test     module=x     teste un module  (ex: module=numeric_methods)
	@echo  mingw32-make test-all              execute tous les tests
	@echo  -------------------------------------------------------
	@echo  mingw32-make new-module name=x     cree un nouveau module (ex: name=gcd)
	@echo  mingw32-make clean                 supprime les fichiers compiles
	@echo  mingw32-make help                  affiche ce message
	@echo.
