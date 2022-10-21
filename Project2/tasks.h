#ifndef __TASKS__H__
#define __TASKS__H__

#include "execute.h"
#include "lexer.h"

using namespace std;

// All the precedence values
typedef enum {
    PREC_LESS, 
    PREC_GREATER, 
    PREC_EQUAL, 
    PREC_ERR, 
    PREC_ACCEPT
} precedenceValue;

// Index based Operator Precedence Table
typedef enum {
    OPP_PLUS,
    OPP_MINUS,
    OPP_MULT,
    OPP_DIV,
    OPP_LPAREN,
    OPP_RPAREN,
    OPP_LBRAC,
    OPP_DOT,
    OPP_RBRAC,
    OPP_NUM,
    OPP_ID,
    OPP_EOE,
} operatorValue;

typedef enum {
    EXPR,
    TERM
} snodeType;

struct stackNode {
    // enum type can be EXPR or TERM
    snodeType type;

    // variable u can hold either expr or term but not both
    union {
        struct exprNode *expr; // this is used when the type is EXPR
        Token *term; // this is used when the type is TERM
    };
};

struct exprNode {
    // enum type: ID_OPER, PLUS_OPER, MINUS__OPER, DIV_OPER
    int curr_operator;

    // ARRAY_ELEM_OPER, WHOLE_ARRAY_OPER
    int type; // type of expression SCALAR, ARRAY, ARRAYDDECL, or ERROR
    
    // these types are discussed later under type checking
    union {
        // operator = ID
        struct {
            string varName;
            int line_no;
        } id;

        // operator = PLUS_OPER, MINUS_OPER
        struct { 
            // MULT_OPET or ARRAY_ELEM_OPER
            struct treeNode *left;
            struct treeNode *right;
        } child;

        // operator = WHOLE_ARRAY_OPER
        struct { 
            string arrayName;
            int line_no;
        } array;
    };
};

stackNode parse_expr();
void parse_assign_stmt();
void parse_braces();
void parse_scalar();
void parse_array();

void parse_and_generate_AST();
void parse_and_type_check();
instNode* parse_and_generate_statement_list();

#endif
