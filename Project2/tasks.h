#ifndef __TASKS__H__
#define __TASKS__H__

#include "execute.h"
#include "lexer.h"
#include <map>

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
map<int, int> map_tokentype_indextable;

typedef enum {
    EXPR,
    TERM
} snodeType;

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

    exprNode() {

    }
};

// Used to map to operatorValue
string symbolMap[12] = {
    "PLUS",
    "MINUS",
    "MULT",
    "DIV",
    "LPAREN",
    "RPAREN",
    "LBRAC",
    "DOT",
    "RBRAC",
    "NUM",
    "ID",
    "END_OF_FILE"
};

int precedence_table[12][12] = {
    {PREC_GREATER, PREC_GREATER, PREC_LESS, PREC_LESS, PREC_LESS, PREC_GREATER, PREC_LESS, PREC_ERR, PREC_GREATER, PREC_LESS, PREC_LESS, PREC_GREATER},
    {PREC_GREATER, PREC_GREATER, PREC_LESS, PREC_LESS, PREC_LESS, PREC_GREATER, PREC_LESS, PREC_ERR, PREC_GREATER, PREC_LESS, PREC_LESS, PREC_GREATER},
    {PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_LESS, PREC_GREATER, PREC_LESS, PREC_ERR, PREC_GREATER, PREC_LESS, PREC_LESS, PREC_GREATER},
    {PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_LESS, PREC_GREATER, PREC_LESS, PREC_ERR, PREC_GREATER, PREC_LESS, PREC_LESS, PREC_GREATER},
    {PREC_LESS, PREC_LESS, PREC_LESS, PREC_LESS, PREC_LESS, PREC_EQUAL, PREC_LESS, PREC_ERR, PREC_LESS, PREC_LESS, PREC_LESS, PREC_ERR},
    {PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_ERR, PREC_GREATER, PREC_GREATER, PREC_ERR, PREC_GREATER, PREC_ERR, PREC_ERR, PREC_GREATER},
    {PREC_LESS, PREC_LESS, PREC_LESS, PREC_LESS, PREC_LESS, PREC_LESS, PREC_LESS, PREC_EQUAL, PREC_EQUAL, PREC_LESS, PREC_LESS, PREC_ERR},
    {PREC_ERR, PREC_ERR, PREC_ERR, PREC_ERR, PREC_ERR, PREC_ERR, PREC_ERR, PREC_ERR, PREC_EQUAL, PREC_ERR, PREC_ERR, PREC_ERR},
    {PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_ERR, PREC_GREATER, PREC_GREATER, PREC_ERR, PREC_GREATER, PREC_ERR, PREC_ERR, PREC_GREATER},
    {PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_ERR, PREC_GREATER, PREC_GREATER, PREC_ERR, PREC_GREATER, PREC_ERR, PREC_ERR, PREC_GREATER},
    {PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_GREATER, PREC_ERR, PREC_GREATER, PREC_GREATER, PREC_ERR, PREC_GREATER, PREC_ERR, PREC_ERR, PREC_GREATER},
    {PREC_LESS, PREC_LESS, PREC_LESS, PREC_LESS, PREC_LESS, PREC_ERR, PREC_LESS, PREC_ERR, PREC_ERR, PREC_LESS, PREC_LESS, PREC_ACCEPT},
};

string valid_rhs[9] = {
    "E - E",
    "E + E",
    "E * E",
    "E / E",
    "LPAREN E RPAREN",
    "E LBRAC E RBRAC",
    "E LBRAC DOT RBRAC",
    "ID",
    "NUM",
};

void duplicate_token(stackNode, Token);
exprNode* parse_expr();
void operator_precedence_parsing(stackNode);
void print_abstract_syntax_tree();
stackNode stack_peeker_top();
stackNode stack_peeker();
void parse_assign_stmt();
void parse_block();
void parse_scalar();
void parse_array();
bool rhs_match(string);

void parse_and_generate_AST();
void parse_and_type_check();
instNode* parse_and_generate_statement_list();

#endif
