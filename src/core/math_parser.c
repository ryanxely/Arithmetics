#include "math_parser.h"

/**
 * @brief Implementation of math_parser function.
 */

Node *make_node(char *expr){
    return Node *decode_expression(*expr);
}

Node *decode_expression(*expr){
    Node *left =
}

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

function apply_binary_operator(float left, char *op, float right){
    switch(op){
        case "+": return left + right; break;
        case "-": return left - right; break;
        case "*": return left * right; break;
        case "/": return left / right; break;
        default: return right;
    }
}