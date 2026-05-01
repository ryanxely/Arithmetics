# README.md

# Project Title

Arithmetics in C

# Description

This project is intended to promote several arithmetic concepts and functions reusable for projects and university students

# About the Author
  - **Name :** Ryan Axel
  - **Email :** ryanxely@gmail.com
  - **Phone :** +237681803185

# Note for users

## Builds (/build)

  - **IO (CLI)**

The CLI builds are made up release files for each module

  - **Release**

For the moment the release is the optimized CLI

### How to use it


# Note for developers

## Includes (/include)

  - **Core Files**

  These files not only show the structure of the modules but also give info about the exposed functions used by external modules like gui or other integrations

## Source Files (/src)

  - **Core Files**

Here we have the implementation of different modules.

  - **IO (CLI)**

    - Each source file corresponds to a cli build. It is the entry point for any aspect of interest of the program (consistent program functionality). In out case, the program is composed of modules, which will hence be seperated for cli in **/seperate-files**
    - We'll also have a final full version in an interactive shell


  - **GUI**

Here we combine the whole project structure as a single program.

### How to use it

# Left Overs

  - **src/core/organiser.c** : Still thinking about how this will be useful for our project. Willing to organise all modules and get an organiser which the full releases (gui app, arithmetics.dll, arithmetics.wasm, arithmetics.js, etc...) will use to organise the modules

  - Still searching for a means to prevent future operations on newton_raphson once an expression error has been detected on the input function