#ifndef MATH_PARSER_H
#define MATH_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Function description for math_parser.
 */

#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    Node *left,
    Token *self,
    Node *right
} Node;

typedef struct {
    char *val,
    TokenType *type
} Token

typedef enum {
    VAR,
    NUM,
    FUNC,
    OP1,
    OP2
} TokenType

typedef enum {
    "sin",
    "cos",
    "tan",
    "log",
    "ln"
} Function



float parse_expression(const p* Parser){

}

#ifdef __cplusplus
}
#endif

#endif // MATH_PARSER_H
