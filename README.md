# Arithmétique

Une bibliothèque mathématique numérique modulaire et une application construites en C/C++, développées comme projet projet du cours INF448. Le projet fournit des implémentations réutilisables des méthodes numériques de base, exposées via une CLI, une interface graphique moderne et des bibliothèques partagées pour l'intégration externe.

---

## Fonctionnalités

- **Détecteur de racines Newton-Raphson** — recherche itérative de racines avec sortie de tableau d'itération complet
- **Intégration numérique des fonctions** — calcule l'intégration d'une fonction avec les méthodes trapèze et simpson
- **Analyseur d'expressions mathématiques** — analyse et évalue les expressions arbitraires en `x` (supporte `sin`, `cos`, `tan`, `log`, `ln`, `^`, multiplication implicite)
- **Visualiseur AST** — affiche graphiquement l'arborescence d'analyse de toute expression
- **Interface graphique moderne** — construite avec Dear ImGui + SDL2 + OpenGL3, avec historique d'opération persistant
- **Cibles de construction multiples** — CLI, GUI, bibliothèque partagée (.dll), bibliothèque statique (.a), WebAssembly (.js)

---

## Prérequis

Les outils suivants doivent être installés sur votre système avant la compilation :

| Outil | Objectif | Installation |
|---|---|---|
| **MinGW-w64** (gcc/g++) | Compilateur C/C++ | [winget install MinGW.MinGW](https://winget.run/) ou [mingw-w64.org](https://www.mingw-w64.org/) |
| **Git** | Contrôle de version + sous-modules | [git-scm.com](https://git-scm.com/) |
| **curl** | Téléchargement de SDL2 lors de l'initialisation | Fourni avec Windows 10+ et Git Bash |
| **mingw32-make** | Système de compilation | Inclus avec MinGW |

> **Emscripten** n'est requis que pour la cible `wasm`. **SDL2** et **ImGui** sont installés automatiquement par `mingw32-make init`.

---

## Avis aux testeurs/utilisateurs

Le dossier `build/` contient les sorties de différents types :

- `build/cli/arithmetics.exe` : Exécutable CLI AIO (All-In-One), qui assemble tous les modules intégrés dans ce projet, à lancer depuis un terminal pour tester les commandes interactives.
> _Remarque: La nouvelle mise à jour des intégration numériques n'est pas encore intégrée dans l'executable AIO_
- `build/cli/seperate-files/` : Exécutables CLI séparés par modules.
- `build/gui/arithmetics.exe` : Application graphique, à ouvrir depuis `build/gui/`.
> _Remarque: La bibliothèque `build/gui/SDL2.dll` doit toujours être dans le même repertoire que l'appli GUI lors de l'execution, au cas où vous désirez télécharger l'appli dans vos machines_
- `build/dll/arithmetics.dll` : Bibliothèque partagée, utilisable depuis un programme C/C++ ou via des bindings externes.
- `build/lib/arithmetics.a` : Bibliothèque statique, à lier dans un projet C/C++ en ajoutant `include/core/` aux chemins d'inclusion.
- `build/lib/arithmetics.js` : Module WebAssembly (si compilé), à importer depuis une page web ou un projet JS/WASM.


## Avis aux contributeurs

### 1. Cloner le référentiel

```bash
git clone https://github.com/ryanxely/arithmetics.git
cd arithmetics
```

### 2. Initialiser l'environnement

Exécutez ceci **une seule fois** après le clonage. Il télécharge ImGui (sous-module git) et SDL2, et les place dans `libs/`:

```bash
mingw32-make init
```

Cela va :
- Initialiser `libs/imgui/` via sous-module git
- Télécharger le paquet SDL2 2.30.3 MinGW depuis les versions officielles de SDL sur GitHub
- Extraire les en-têtes, les fichiers lib et `SDL2.dll` dans `libs/SDL2/`

### 3. Compiler

```bash
# Compiler la CLI interactive (par défaut)
mingw32-make

# Compiler l'application GUI
mingw32-make gui

# Exécuter la GUI
./build/gui/arithmetics.exe
```

---

## Cibles de compilation

| Commande | Résultat | Description |
|---|---|---|
| `mingw32-make` | `build/cli/arithmetics.exe` | CLI interactive (tous les modules) |
| `mingw32-make cli module=x` | `build/cli/seperate-files/x.exe` | CLI pour un module spécifique |
| `mingw32-make gui` | `build/gui/arithmetics.exe` | Application graphique |
| `mingw32-make dll` | `build/dll/arithmetics.dll` | Bibliothèque partagée pour Python/web |
| `mingw32-make lib` | `build/lib/arithmetics.a` | Bibliothèque statique |
| `mingw32-make wasm` | `build/lib/arithmetics.js` | WebAssembly (nécessite Emscripten) |
| `mingw32-make debug module=x` | `build/debug/...` | Compilation de débogage avec symboles |
| `mingw32-make release module=x` | `build/release/...` | Compilation optimisée pour la version finale |
| `mingw32-make test module=x` | exécute le test | Tester un module spécifique |
| `mingw32-make test-all` | exécute tous les tests | Exécuter tous les tests de module |
| `mingw32-make new-module name=x` | génère les fichiers | Créer un squelette de nouveau module |
| `mingw32-make clean` | — | Supprimer toute la sortie compilée |
| `mingw32-make help` | — | Afficher toutes les commandes disponibles |

---

## Structure fonctionnelle du projet

```
arithmetics/
│
├── include/core/          En-têtes — API publique de chaque module
│   ├── math_parser.h      Types et fonctions de l'analyseur d'expressions
│   ├── numeric_methods.h  Types et fonctions de Newton-Raphson
│   ├── numeric_integration.h  Types et fonctions de l'intégration numérique (trapèze et simpson)
│   └── utilities.h        Utilitaires de chaîne (substr, concat, contains)
│
├── src/core/              Implémentations — mathématiques pures, pas d'UI, pas d'E/S
│   ├── math_parser.c      Analyseur de descente récursive + évaluateur AST
│   ├── numeric_methods.c  Newton-Raphson avec capture d'itération complète
│   ├── numeric_methods.c  Implémentation des méthodes d'intégration numérique
│   └── utilities.c        Fonctions d'assistance en chaîne
│
├── src/io/                Points d'entrée CLI
│   ├── main_index.cpp     Shell interactif tout-en-un
│   └── numeric_methods_io.c  CLI autonome pour les méthodes numériques
│   └── numeric_integration_io.c  CLI autonome pour l'intégration numérique
│
├── src/ui/                Application GUI
│   └── main.cpp    Application Dear ImGui (tableau de bord, modules, historique)
│
├── tests/                 Tests unitaires
│   ├── test_numeric_methods.cpp
│   ├── test_numeric_integration.cpp
│   ├── test_math_parser.cpp
│   └── test_utilities.cpp
│
├── libs/                  Bibliothèques tierces (remplies par init)
│   ├── imgui/             Dear ImGui (sous-module git)
│   └── SDL2/              En-têtes SDL2, lib, dll
│
├── bindings/              Modèles de pont de langage
│
├── scripts/               Automatisation pour développeurs
│   └── new_module.sh      Génère les fichiers .h, .c et de test pour un nouveau module
│
├── build/                 Sortie compilée
├── docs/                  Documentation du projet
├── Makefile               Fichier de compilation principal
└── README.md              Ce fichier
```

---

## Ajouter un nouveau module

Utilisez le script fourni pour générer automatiquement les fichiers squelette (exp. gcd):

```bash
mingw32-make new-module name=gcd 
```

Cela crée :
- `include/core/gcd.h` — en-tête avec signatures de fonction documentées
- `src/core/gcd.c` — modèle de mise en œuvre
- `tests/test_gcd.cpp` — modèle de fichier de test

Ensuite, ajoutez `src/core/gcd.c` à la variable `CORE` dans le `Makefile` et vous êtes prêt à compiler.

---

## Architecture

Le projet suit une séparation stricte des préoccupations dans trois zones :

**Core** (`src/core/`) — calcul pur, pas de sortie, pas de dépendances sur l'UI ou l'E/S. Chaque algorithme vit ici en tant que module autonome. Cette couche peut être compilée en tant que bibliothèque partagée et appelée depuis Python, JavaScript (via WebAssembly) ou tout autre langage.

**Interface** (`src/io/`, `src/ui/`) — consomme le cœur. Les couches CLI et GUI appelent les fonctions de base et présentent les résultats. Elles ne contiennent jamais de logique mathématique.

**Tests** (`tests/`) — vérifient le cœur en isolation, avant que toute interface ne soit impliquée.

---

## Limitations connues / Travaux en cours

- La détection d'erreur d'expression lors de la validation des entrées Newton-Raphson est encore en cours de raffinage
- La cible WebAssembly nécessite l'installation séparée d'Emscripten

---

## Auteurs

| **ID** | **Nom** |
|---|---|
| 22S74587 | TCHEUTCHOUA FONGUELE RYAN AXEL |
| 22S74303 | NGON TAKOM YVES-MARTIN |
| 22S74125 | MBIADJEU NANA KARL-ANTHONY |

> _Faculté des Sciences - Douala - 2025/2026_
> _Sous la supervision de **Dr Noumsi Auguste**_

