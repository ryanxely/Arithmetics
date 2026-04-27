#include "math_parser.h"

/**
 * @brief Implementation of math_parser function.
 */

// Constructing the AST

Node* create_node(char *str, TokenType type){
    Node* n = malloc(sizeof(Node));
    n->left = NULL;
    n->right = NULL;
    n-self->val = str;
    n->self->type = type;
}

Node *make_node(char *expr){
    int i = 0;
    return Node *decode_expression(expr, &i);
}

Node *decode_expression(char *expr, int *i){
    Node *left = decode_term(expr, i);
    *i++;
    if (expr[*i] == "+" || expr[*i] == "-"){
        Node *node = create_node(expr[*i], OP2);
        node->left = left;
        *i++;
        node->right = decode_expression(expr, i);
        return node;
    } else if (expr[*i] == '\0') {
        return left;
    } else return create_node(concat("Unexpected token at position ", *i, " : ", expr[*i]), ERR);
}

Node *decode_term(char *expr, int *i){
    Node *left = decode_factor(expr, i);
    *i++;
    if (expr[*i] == "*" || expr[*i] == "/"){
        Node *node = create_node(expr[*i], OP2);
        node->left = left;
        *i++;
        node->right = decode_expression(expr, i);
        return node;
    } else if (expr[i] == '\0') {
        return left;
    } else return create_node(concat("Unexpected token at position ", *i, " : ", expr[*i]), ERR);
}

Node *decode_factor(char *expr, int *i){
    Node *left = decode_value(expr, i);
    *i++;
    if (expr[*i] == "^"){
        Node *node = create_node(expr[*i], OP2);
        node->left = left;
        *i++;
        node->right = decode_expression(expr, i);
        return node;
    } else if (expr[i] == '\0') {
        return left;
    } else return create_node(concat("Unexpected token at position ", *i, " : ", expr[*i]), ERR);
}

Node *decode_value(char *expr, int *i){
    if (expr[*i] == "+" || expr[*i] == "-"){
        Node *node = create_node(expr[*i], OP1);
        *i++;
        node->right = decode_expression(expr, i);
        return node;
    } else if (isalpha(expr[*i])){
        int start = *i;
        while(isalpha(expr[*i])){continue;}
        char *func = substring(expr, start, *i-start);
        Node *node = contains(FUNCTIONS, func) ? create_node(func, FUNC) : (*i - start == 1) ? create_node(func, VAR) : create_node(concat("Unexpected Token : ", func), ERR);
        *i++;
        node->right = decode_expression(expr, i);
        return node;
    } else if (expr[*i] == "("){
        int start = ++(*i);
        int n_braces = 1;
        while(expr[*i++] != "\0" && n_braces > 0) {
            if (expr[*i] == "(") n_braces++;
            else if (expr[*i] == ")") n_braces--;
        }
        if (expr[*i++] == "\0" && n_braces > 0) Node *node = create_node("Expected )", ERR);
        else {
            char* sub_expr = substring(expr, start, *i-start);
            return evaluate_expression(sub_expr);
        }
    } else if (isnumber) {
        return number;
    } else return create_node(concat("Unexpected Token: ", expr[*i]), ERR);
}



// Evaluating the created node

void evaluate_expression(char *expr, float x){
    Node *N = make_node(*expr)
    return evaluate_node(*N, x)
}

float evaluate_node(Node N, float x){
    switch((N.self).type){
        case NUM: return (N.self).val; break;
        case VAR: return x; break;
        case FUNC: return apply_function((N.self).val, evaluate_node((N.self).right)); break;
        case OP1: return apply_unary_operator((N.self).val, evaluate_node((N.self).right)); break;
        case OP2: return apply_binary_operator(evaluate_node((N.self).left), (N.self).val, evaluate_node((N.self).right)); break;
        default: return 0;
    }
}

float apply_function(char *func, float val){
    switch(func){
        case "sin": return math.sin(val); break;
        case "cos": return math.cos(val); break;
        case "tan": return math.tan(val); break;
        case "log": return math.log(val); break;
        case "ln": return math.ln(val); break;
        default: return val;
    }
}

float apply_unary_operator(char *op, char val){
    switch(op){
        case "+": return val; break;
        case "-": return -1*val; break;
        default: return val
    }
}

float apply_binary_operator(float left, char *op, float right){
    switch(op){
        case "+": return left + right; break;
        case "-": return left - right; break;
        case "*": return left * right; break;
        case "/": return left / right; break;
        default: return right;
    }
}