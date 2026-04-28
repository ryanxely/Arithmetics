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
    union{
        char* c_val,
        float n_val
    },
    TokenType *type
} Token

typedef enum {
    VAR,
    NUM,
    FUNC,
    OP1,
    OP2,
    ERR
} TokenType

Node* tokenize(char *expr);

Node* create_node(char *val, TokenType type);
Node* create_node(float val, TokenType type);

Node *parse_expression(char *expr, int *i);
Node* parse_term(char *expr, int *i);
Node *parse_factor(char *expr, int *i)

void evaluate_expression(char *expr, float x);
float evaluate_node(Node N, float x);
float apply_function(char *func, float val);
float apply_unary_operator(char *op, char val);
float apply_binary_operator(float left, char *op, float right);



#ifdef __cplusplus
}
#endif

#endif // MATH_PARSER_H
