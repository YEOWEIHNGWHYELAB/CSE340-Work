#pragma once

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <stack>
#include <string>
#include <unordered_set>
#include <vector>
#include "execute.h"
#include "inputbuf.h"
#include "lexer.h"
#include "tasks.h"

using namespace std;


typedef enum {
    EXPR,
    TERM
} snodeType;

// All the precedence values
typedef enum {
    PREC_LESS, 
    PREC_GREATER, 
    PREC_EQUAL, 
    PREC_ERR, 
    PREC_ACCEPT
} precedenceValue;

typedef enum {
    SCALAR_TYPE,
    ARRAYDDECL_TYPE,
    ARRAY_TYPE,
    ERROR_TYPE
} exprNodeType;

// Either expression or a term
typedef struct stackNode {
    // enum type can be EXPR or TERM
    snodeType type;

    // this is used when the type is EXPR
    struct exprNode *expr;
    
    // this is used when the type is TERM
    Token term;

    stackNode() {};
} stackNode;

typedef enum {
    ID_OPER,
    NUM_OPER, 
    PLUS_OPER, 
    MINUS_OPER,
    MULT_OPER,
    DIV_OPER,
    EQUAL_OPER,
    ARRAY_ELEM_OPER, 
    WHOLE_ARRAY_OPER,
    EXPR_OPER // (EXPR)
} operatorType;

// Strictly of type expression
typedef struct exprNode {
    // enum type: ID_OPER, PLUS_OPER, MINUS_OPER, DIV_OPER, ARRAY_ELEM_OPER, WHOLE_ARRAY_OPER
    operatorType curr_operator;

    // type of expression SCALAR, ARRAYDDECL, ARRAY, or ERROR
    exprNodeType type;
    
    // These types below are discussed later under type checking

    // operator = ID_OPER or NUM_OPER
    struct id {
        string varName;
        int line_no;
    } id;
    
    // operator = EQUAL_OPER, PLUS_OPER, MINUS_OPER, DIV_OPER, MULT_OPET, ARRAY_ELEM_OPER or WHOLE_ARRAY_OPER
    struct child { 
        struct exprNode *left;
        struct exprNode *right;
    } child;

    struct array {
        string arrayName;
        int line_no;
    } array;

    exprNode() {};
} exprNode;

void initialize_map();
void intialize_rhs();

Token peek_symbol();
Token get_symbol();

// For printing AST
void expr_node_printer(exprNode*);
void print_abstract_syntax_tree();

// Lexer additional function
Token expect(TokenType);
Token terminal_peek(vector<stackNode*>);

// Parser
exprNode* parse_expr();
exprNode* parse_variable_access();
exprNode* parse_output_stmt();
exprNode* parse_assign_stmt();
void parse_stmt();
void parse_stmt_list();
void parse_block();
void parse_id_list();
void parse_decl_section();
void parse_input();
void parse_task_1();

bool has_type_error();
bool has_assignment_error();
void print_type_error_line();
void print_assignment_error_line();
