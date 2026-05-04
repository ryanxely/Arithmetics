#ifndef MATH_PARSER_H
#define MATH_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Data Structures*/
typedef enum ParserTokenType {
    NUM,   /* numeric literal */
    VAR,   /* variable x      */
    FUNC,  /* sin/cos/…       */
    OP1,   /* unary  +/-      */
    OP2,   /* binary: + - * / ^ */
    ERR    /* parse error     */
} ParserTokenType;

typedef struct Token {
    ParserTokenType type;
    char*     c_val;   /* operator / function name / error message */
    float     n_val;   /* numeric value (NUM nodes)                */
} Token;

typedef struct Node {
    Token*       self;
    struct Node* left;
    struct Node* right;
} Node;

/* Public API */
Node*  tokenize(char* expr);
float  evaluate_expression(char* expr, float x);
void   display_tree(Node* n);

/* Internal helpers exposed for newton_raphson */
float  evaluate_node(Node* n, float x);
void   free_tree(Node* n);

#ifdef __cplusplus
}
#endif

#endif