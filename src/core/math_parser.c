#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "core/math_parser.h"
#include "core/utilities.h"

// ——————————————————————
/*  Known function names                                               */
// ——————————————————————

#define N_FUNCTIONS 5
static const char* FUNCTIONS[N_FUNCTIONS] = {
    "sin", "cos", "tan", "log", "ln"
};

// ——————————————————————
/*  Forward declarations for the recursive-descent parser             */
// ——————————————————————

static Node* parse_expression(const char* expr, int* i);
static Node* parse_term     (const char* expr, int* i);
static Node* parse_factor   (const char* expr, int* i);

// ——————————————————————
/*  Node constructors                                                  */
// ——————————————————————

/* Create a node carrying a string value (operator / function / error). */
static Node* create_node_str(const char* val, TokenType type) {
    Node*  n = (Node*)malloc(sizeof(Node));
    Token* t = (Token*)malloc(sizeof(Token));
    if (!n || !t) { free(n); free(t); return NULL; }
    t->type  = type;
    t->c_val = val ? strdup(val) : NULL;
    t->n_val = 0.0f;
    n->self  = t;
    n->left  = NULL;
    n->right = NULL;
    return n;
}

/* Create a node carrying a numeric value. */
static Node* create_node_num(float val) {
    Node*  n = (Node*)malloc(sizeof(Node));
    Token* t = (Token*)malloc(sizeof(Token));
    if (!n || !t) { free(n); free(t); return NULL; }
    t->type  = NUM;
    t->c_val = NULL;
    t->n_val = val;
    n->self  = t;
    n->left  = NULL;
    n->right = NULL;
    return n;
}

// ——————————————————————
/*  Helper: skip whitespace                                            */
// ——————————————————————

static void skip_ws(const char* expr, int* i) {
    while (expr[*i] == ' ' || expr[*i] == '\t') (*i)++;
}


// ——————————————————————
/*  Recursive-descent parser                                           

    Grammar (simplified, right-associative for demo):                 
    expression = term  { (’+’|’-’) term }
    term       = factor { (’*’|’/’|’^’) factor }
    factor     = [unary] ( number | variable | function ‘(’ expr ‘)’ | ‘(’ expr ‘)’ ) */
// —————————————————————— 


/**
 * @brief parse_expression: handles + and - (lowest precedence)
 */
static Node* parse_expression(const char* expr, int* i) {
    skip_ws(expr, i);
    Node* left = parse_term(expr, i);
    if (!left || left->self->type == ERR) return left;

    skip_ws(expr, i);
    while (expr[*i] == '+' || expr[*i] == '-') {
        char op[2] = { expr[*i], '\0' };
        (*i)++;
        skip_ws(expr, i);

        Node* node  = create_node_str(op, OP2);
        node->left  = left;
        node->right = parse_term(expr, i);
        if (!node->right || node->right->self->type == ERR) {
            /* propagate error */
            Node* err = node->right;
            node->right = NULL;
            free_tree(node);
            return err ? err : create_node_str("Fin de l'expression inattendue", ERR);
        }
        left = node;
        skip_ws(expr, i);
    }

    return left;
}

/**
 * @brief parse_term: handles *, /, ^ (higher precedence than +/-)
 */
static Node* parse_term(const char* expr, int* i) {
    skip_ws(expr, i);
    Node* left = parse_factor(expr, i);
    if (!left || left->self->type == ERR) return left;
    
    skip_ws(expr, i);
    /* Explicit operators */
    while (expr[*i] == '*' || expr[*i] == '/' || expr[*i] == '^') {
        char op[2] = { expr[*i], '\0' };
        (*i)++;
        skip_ws(expr, i);
        
        Node* node  = create_node_str(op, OP2);
        node->left  = left;
        node->right = parse_factor(expr, i);
        if (!node->right || node->right->self->type == ERR) {
            Node* err = node->right;
            node->right = NULL;
            free_tree(node);
            return err ? err : create_node_str("Fin de l'expression inattendue", ERR);
        }
        left = node;
        skip_ws(expr, i);
    }
    
    /* Implicit multiplication: e.g. "2x", "2(x+1)", "x(x+1)" */
    while (isalpha((unsigned char)expr[*i]) || expr[*i] == '(') {
        Node* node  = create_node_str("*", OP2);
        node->left  = left;
        node->right = parse_factor(expr, i);
        if (!node->right || node->right->self->type == ERR) {
            Node* err = node->right;
            node->right = NULL;
            free_tree(node);
            return err ? err : create_node_str("Fin de l'expression inattendue", ERR);
        }
        left = node;
        skip_ws(expr, i);
    }
    
    return left;
}

/**
 * @brief parse_factor: handles unary +/-, numbers, variables, functions, parentheses.
 */
static Node* parse_factor(const char* expr, int* i) {
    skip_ws(expr, i);
    
    /* — Unary +/- — */
    if (expr[*i] == '+' || expr[*i] == '-') {
        char op[2] = { expr[*i], '\0' };
        (*i)++;
        Node* node  = create_node_str(op, OP1);
        node->right = parse_factor(expr, i);   /* recursive for e.g. -x */
        return node;
    }
    
    /* — Alphabetic: function name or variable — */
    if (isalpha((unsigned char)expr[*i])) {
        int start = *i;
        while (isalpha((unsigned char)expr[*i])) (*i)++;
        int   word_len = *i - start;
        char* word     = substr(expr, start, word_len);
        
        if (contains(word, FUNCTIONS, N_FUNCTIONS)) {
            /* function call: must be followed by '(' */
            Node* node = create_node_str(word, FUNC);
            free(word);
            skip_ws(expr, i);
            if (expr[*i] != '(') {
                free_tree(node);
                return create_node_str("Paranthese '(' requise apres le nom de la fonction", ERR);
            }
            (*i)++;   /* consume '(' */
            node->right = parse_expression(expr, i);
            skip_ws(expr, i);
            if (expr[*i] != ')') {
                free_tree(node);
                return create_node_str("Paranthese ')' requise apres l'argument de la fonction", ERR);
            }
            (*i)++;   /* consume ')' */
            return node;
        } else if (word_len == 1 && word[0] == 'x') {
            Node* node = create_node_str("x", VAR);
            free(word);
            return node;
        } else {
            char* msg = concat("Identifiant inconnu: ", word, NULL);
            free(word);
            Node* err = create_node_str(msg, ERR);
            free(msg);
            return err;
        }
        
    }

    /* — Parenthesised sub-expression — */
    if (expr[*i] == '(') {
        (*i)++;   /* consume '(' */
        Node* inner = parse_expression(expr, i);
        skip_ws(expr, i);
        if (expr[*i] != ')') {
            free_tree(inner);
            return create_node_str("Paranthese ')' requise", ERR);
        }
        (*i)++;   /* consume ')' */
        return inner;
    }
    
    /* — Numeric literal (integers and decimals) — */
    if (isdigit((unsigned char)expr[*i]) || expr[*i] == '.') {
        char  buf[64];
        int   j = 0;
        while ((isdigit((unsigned char)expr[*i]) || expr[*i] == '.') && j < 63) {
            buf[j++] = expr[(*i)++];
        }
        buf[j] = '\0';
        float val = (float)atof(buf);
        return create_node_num(val);
    }
    
    /* — Anything else is an error — */
    if (expr[*i] == '\0') {
        return create_node_str("Fin de l'expression inattendue", ERR);
    }
    char unexpected[3] = { expr[*i], '\0' };
    char* msg = concat("Jeton inattendu: ", unexpected, NULL);
    Node* err = create_node_str(msg, ERR);
    free(msg);
    (*i)++;
    return err;
}

// ——————————————————————
/*  Public: tokenize (build AST)                                       */
// ——————————————————————

Node* tokenize(char* expr) {
    int i = 0;
    return parse_expression(expr, &i);
}

// ——————————————————————
/*  Tree memory management                                             */
// ——————————————————————

void free_tree(Node* n) {
    if (!n) return;
    free_tree(n->left);
    free_tree(n->right);
    if (n->self) {
        free(n->self->c_val);
        free(n->self);
    }
    free(n);
}

// void display_tree(Node* n) {
    
// }

// ——————————————————————
/*  Forward declarations for the recursive-ascent evaluation helpers           */
// ——————————————————————

// static float apply_function(const char* func, float val);
// static float apply_unary(const char* op, float val);
// static float apply_binary(float left, const char* op, float right);

// ——————————————————————
/*  Evaluation helpers                                                   */
// ——————————————————————

static float apply_function(const char* func, float val) {
    val = val * 3.1412f / 180.0f;  /* Convert degrees to radians for trig functions */
    if (strcmp(func, "sin") == 0) return sinf(val);
    if (strcmp(func, "cos") == 0) return cosf(val);
    if (strcmp(func, "tan") == 0) return tanf(val);
    if (strcmp(func, "log") == 0) return log10f(val);
    if (strcmp(func, "ln")  == 0) return logf(val);
    return val;
}

static float apply_unary(const char* op, float val) {
    if (op[0] == '-') return -val;
    return val;   /* '+' */
}

static float apply_binary(float left, const char* op, float right) {
    switch (op[0]) {
        case '+': return left + right;
        case '-': return left - right;
        case '*': return left * right;
        case '/': return (right != 0.0f) ? left / right : NAN;
        case '^': return powf(left, right);
        default:  return right;
    }
}

// ——————————————————————
/*  Public: evaluate_node                                              */
// ——————————————————————

float evaluate_node(Node* n, float x) {
    if (!n) return 0.0f;
    TokenType type = n->self->type;

    switch (type) {
        case NUM:  return n->self->n_val;
        case VAR:  return x;
        case FUNC: return apply_function(n->self->c_val, evaluate_node(n->right, x));
        case OP1:  return apply_unary(n->self->c_val, evaluate_node(n->right, x));
        case OP2:  return apply_binary(evaluate_node(n->left,  x), n->self->c_val, evaluate_node(n->right, x));
        case ERR:
            fprintf(stderr, "Erreur de syntaxe: %s\n", n->self->c_val);
            return NAN;
        default:   return 0.0f;
    }

}

// ——————————————————————
/*  Public: evaluate_expression                                        */
// ——————————————————————

float evaluate_expression(char* expr, float x){
    Node* tree   = tokenize(expr);
    float result = evaluate_node(tree, x);
    free_tree(tree);
    return result;
}
