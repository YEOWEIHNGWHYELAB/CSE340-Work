#ifndef __TASKS__H__
#define __TASKS__H__

#include "execute.h"
#include "lexer.h"
#include <map>
#include "error.h"

using namespace std;

// All the precedence values
typedef enum {
    PREC_LESS, 
    PREC_GREATER, 
    PREC_EQUAL, 
    PREC_ERR, 
    PREC_ACCEPT
} precedenceValue;

typedef enum {
    EXPR,
    TERM
} snodeType;

typedef enum {
    SCALAR_TYPE, 
    ARRAY_TYPE, 
    ARRAYDDECL_TYPE, 
    ERROR_TYPE
} exprNodeType;

typedef enum {
    ID_OPER,
    NUM_OPER, 
    PLUS_OPER, 
    MINUS_OPER,
    MULT_OPER,
    DIV_OPER,
    EQUAL_OPER,
    ARRAY_ELEM_OPER, 
    WHOLE_ARRAY_OPER
} operatorType;

// Either expression or a term
struct stackNode {
    // enum type can be EXPR or TERM
    snodeType type;

    // variable u can hold either expr or term but not both
    union {
        struct exprNode *expr; // this is used when the type is EXPR
        Token *term; // this is used when the type is TERM
    };
};

// Strictly of type expression
struct exprNode {
    // enum type: ID_OPER, PLUS_OPER, MINUS_OPER, DIV_OPER, ARRAY_ELEM_OPER, WHOLE_ARRAY_OPER
    operatorType curr_operator;

    // type of expression SCALAR, ARRAY, ARRAYDDECL, or ERROR
    exprNodeType type;
    
    // these types are discussed later under type checking
    union {
        // operator = ID_OPER
        struct {
            string varName;
            int line_no;
        } id;

        // operator = PLUS_OPER, MINUS_OPER, DIV_OPER, MULT_OPET or ARRAY_ELEM_OPER
        struct {
            struct exprNode *left;
            struct exprNode *right;
        } child;

        // operator = WHOLE_ARRAY_OPER
        struct { 
            string arrayName;
            int line_no;
        } array;
    };

    // ID_OPER
    exprNode(operatorType operator_type, exprNodeType expr_type, string var_name, int line_num) {
        this->curr_operator = operator_type;
        this->type = expr_type;

        if (this->curr_operator == ID_OPER) {
            id.varName = var_name;
            id.line_no = line_num;
        } else if (this->curr_operator == WHOLE_ARRAY_OPER) {
            array.arrayName = var_name;
            array.line_no = line_num;
        } else {
            syntax_error();
        }
    }

    // WHOLE_ARRAY_OPER
    exprNode(operatorType operator_type, exprNodeType expr_type, exprNode* left_child) {
        this->curr_operator = operator_type;
        this->type = expr_type;

        if (this->curr_operator == WHOLE_ARRAY_OPER) {
            child.left = left_child;
        } else {
            syntax_error();
        }
    }

    // PLUS_OPER, MINUS_OPER, DIV_OPER, MULT_OPET OR ARRAY_ELEM_OPER
    exprNode(operatorType operator_type, exprNodeType expr_type, exprNode* left_child, exprNode* right_child) {
        this->curr_operator = operator_type;
        this->type = expr_type;

        if (this->curr_operator == PLUS_OPER || this->curr_operator == MINUS_OPER || this->curr_operator == DIV_OPER || this->curr_operator == MULT_OPER || this->curr_operator == ARRAY_ELEM_OPER) {
            child.left = left_child;
            child.right = right_child;
        } else {
            syntax_error();
        }
    }
};

string reverse_rhs_builder(vector<stackNode>);
void duplicate_token(stackNode, Token);
exprNode* parse_expr();
void operator_precedence_parsing(stackNode);
void print_abstract_syntax_tree();
stackNode stack_peeker_top();
stackNode stack_peeker();
exprNode* parse_variable_access();
void parse_assign_stmt();
void parse_stmt_list();
void parse_block();
void parse_scalar();
void parse_array();
bool rhs_match(string);
exprNodeType expr_type();

void parse_and_generate_AST();
void parse_and_type_check();
instNode* parse_and_generate_statement_list();

#endif
