#include <iostream>
#include <vector>
#include <cstdio>
#include <sstream>
#include <cstdlib>
#include "lexer.h"
#include "execute.h"
#include "tasks.h"
#include "error.h"
#include <queue>

using namespace std;

// Note that the stack is where you build the AST!
LexicalAnalyzer lexer;

// Stack to store
vector<stackNode> ast_stack;
vector<stackNode> stack;

// First statement AST
stringstream first_ast;

// Scalar & Vector declare list
vector<Token> scalar_list;
vector<Token> array_list;

// Root of exprNode
exprNode* root;

// Index based Operator Precedence Table
map<TokenType, int> map_tokentype_indextable;

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

// If the RHS is none of this, then it will be syntax error
// But we will also need to store if it is unary or binary 
// and the position of expr 
string valid_rhs[9] = {
    "E-E",
    "E+E",
    "E*E",
    "E/E",
    "(E)",
    "E[E]",
    "E[.]",
    "ID",
    "NUM"
};

string expr_string[9] = {
    "ID",
    "NUM", 
    "+", 
    "-",
    "*",
    "/",
    "=",
    "[]", 
    "[.]"
};

// Initailize the map that maps token type index to the correct index precedence table 
void initialize_map() {
    map_tokentype_indextable.insert(pair<TokenType, int>(PLUS, 0));
    map_tokentype_indextable.insert(pair<TokenType, int>(MINUS, 1));
    map_tokentype_indextable.insert(pair<TokenType, int>(MULT, 2));
    map_tokentype_indextable.insert(pair<TokenType, int>(DIV, 3));
    map_tokentype_indextable.insert(pair<TokenType, int>(LPAREN, 4));
    map_tokentype_indextable.insert(pair<TokenType, int>(RPAREN, 5));
    map_tokentype_indextable.insert(pair<TokenType, int>(LBRAC, 6));
    map_tokentype_indextable.insert(pair<TokenType, int>(DOT, 7));
    map_tokentype_indextable.insert(pair<TokenType, int>(RBRAC, 8));
    map_tokentype_indextable.insert(pair<TokenType, int>(NUM, 9));
    map_tokentype_indextable.insert(pair<TokenType, int>(ID, 10));
    map_tokentype_indextable.insert(pair<TokenType, int>(END_OF_FILE, 11));
}

Token expect(TokenType expected_type) {
    Token curr_token = lexer.GetToken();

    if (curr_token.token_type != expected_type)
        syntax_error();

    return curr_token;
}

Token get_symbol() {
    return lexer.GetToken();
}

Token peek_symbol() {
    Token next_token_t1 = lexer.peek(1);

    // 3 cases of EOE
    // If the next token is SEMICOLON
    // If the next token is RBRAC and the token after that is EQUAL
    // If the next token is RBRAC and the token after that is SEMICOLON
    if (next_token_t1.token_type == SEMICOLON) {
        Token eoe_tok;
        eoe_tok.lexeme = "$";
        eoe_tok.token_type = END_OF_FILE;
        eoe_tok.line_no = next_token_t1.line_no;

        return eoe_tok;
    } else if (next_token_t1.token_type == RBRAC) {
        Token next_token_t2 = lexer.peek(2);

        if (next_token_t2.token_type == EQUAL || next_token_t2.token_type == SEMICOLON) {
            Token eoe_tok;
            eoe_tok.lexeme = "$";
            eoe_tok.token_type = END_OF_FILE;
            eoe_tok.line_no = next_token_t1.line_no;

            return eoe_tok;
        }
    }

    // Otherwise return next symbol
    return next_token_t1;
}

/**
 * Returns the stackNode of type terminal closest to the top of stack or
 * just below top of stack
*/
stackNode stack_peeker() {    
    vector<stackNode>::iterator stack_peeker = stack.end();

    if (stack_peeker->type == TERM) {
        // If the top is a TERM
        // stack_peeker->term->Print();
        return *stack_peeker;
    } else {
        // If top is not a TERM, peek below by one on the stack 
        stack_peeker -= 1;

        // Check if the element below the top of the stack is stack
        if (stack_peeker->type == TERM) {
            // stack_peeker->term->Print();
            return *stack_peeker;
        } else {
            syntax_error();
        }
    }
}

// Reverses the stackNodes built from the RHS Stack and return a string of RHS
string reverse_rhs_builder(vector<stackNode> rhs_stack) {
    stringstream rhs_string_stream;

    for (auto curr_stackNode = rhs_stack.end(); curr_stackNode >= rhs_stack.begin(); curr_stackNode--) {
        if (curr_stackNode->type == TERM) {
            if (curr_stackNode->term->token_type == PLUS) {
                string expr_str = "+";
                rhs_string_stream << expr_str;
            } else if (curr_stackNode->term->token_type == MINUS) {
                string expr_str = "-";
                rhs_string_stream << expr_str;
            } else if (curr_stackNode->term->token_type == MULT) {
                string expr_str = "*";
                rhs_string_stream << expr_str;
            } else if (curr_stackNode->term->token_type == DIV) {
                string expr_str = "/";
                rhs_string_stream << expr_str;
            } else if (curr_stackNode->term->token_type == LPAREN) {
                string expr_str = "(";
                rhs_string_stream << expr_str;
            } else if (curr_stackNode->term->token_type == RPAREN) {
                string expr_str = ")";
                rhs_string_stream << expr_str;
            } else if (curr_stackNode->term->token_type == LBRAC) {
                string expr_str = "[";
                rhs_string_stream << expr_str;
            } else if (curr_stackNode->term->token_type == RBRAC) {
                string expr_str = "]";
                rhs_string_stream << expr_str;
            } else if (curr_stackNode->term->token_type == ID) {
                string expr_str = "ID";
                rhs_string_stream << expr_str;
            } else if (curr_stackNode->term->token_type == NUM) {
                string expr_str = "NUM";
                rhs_string_stream << expr_str;
            } else {
                syntax_error();
            }
        } else {
            string expr_str = "E";
            rhs_string_stream << expr_str;
        }
    }

    return rhs_string_stream.str();
}

// Peek only the top stackNode of the stack
stackNode stack_peeker_top() {
    return *stack.end();
}

// Duplicate the token
void duplicate_token(stackNode src, Token dest) {
    dest.lexeme = src.term->lexeme;
    dest.line_no = src.term->line_no;
    dest.token_type = src.term->token_type;
}

// Duplicate stackNode
void duplicate_stack_node(stackNode src, stackNode dest) {
    if (src.type == TERM) {
        dest.type = TERM;
        dest.term->lexeme = src.term->lexeme;
        dest.term->line_no = src.term->line_no;
        dest.term->token_type = src.term->token_type;
    } else {
        dest.type = EXPR;
        operatorType operator_type = src.expr->curr_operator;
        dest.expr->curr_operator = operator_type;
        dest.expr->type = src.expr->type;

        if (operator_type == PLUS_OPER || operator_type == MINUS_OPER || operator_type == DIV_OPER || operator_type == MULT_OPER || operator_type == EQUAL_OPER || operator_type == ARRAY_ELEM_OPER) {
            dest.expr->child.left = src.expr->child.left;
            dest.expr->child.right = src.expr->child.right;
        } else if (operator_type == WHOLE_ARRAY_OPER) {
            dest.expr->child.left = src.expr->child.left;
            dest.expr->child.right = nullptr;
        } else if (operator_type == NUM_OPER || operator_type == ID_OPER) {
            dest.expr->id.varName = src.expr->id.varName;
            dest.expr->id.line_no = src.expr->id.line_no;
        }
    }
}

// Check if the current rhs to match is valid
bool rhs_match(string curr_rhs) {
    for (string curr_str : valid_rhs) {
        if (curr_rhs == curr_str) 
            return true;
    }

    return false;
}

// Return the type of operator the expression string that is reduced
operatorType operator_type(string curr_rhs) {
    if (curr_rhs == "ID" || curr_rhs == "(E)") {
        return ID_OPER;
    } else if (curr_rhs == "NUM") {
        return NUM_OPER;
    } else if (curr_rhs == "E+E") {
        return PLUS_OPER;
    } else if (curr_rhs == "E-E") {
        return MINUS_OPER;
    } else if (curr_rhs == "E*E") {
        return MULT_OPER;
    } else if (curr_rhs == "E/E") {
        return DIV_OPER;
    } else if (curr_rhs == "E[E]") {
        return ARRAY_ELEM_OPER;
    } else if (curr_rhs == "E[.]") {
        return WHOLE_ARRAY_OPER;
    } else {
        syntax_error();
    }
}

// Takes in the variable name and determine if it is SCALAR, ARRAY, ERROR 
exprNodeType expr_type(string varname) {
    for (auto ii : scalar_list) {
        if (ii.lexeme == varname) 
            return SCALAR_TYPE;
    }

    for (auto ii : array_list) {
        if (ii.lexeme == varname) 
            return ARRAY_TYPE;
    }

    return ERROR_TYPE;
}

/**
 * Stack operations
 * 
 * 1) Peek at the terminal closest to the top this is either the top of 
 * the stack or just below the top of the stack
 * 2) Shift a token on the stack
 * 3) Reduce This one is more involved. There are two parts to this:
 *      1. You should write a function that pops elements from the stack 
 *      until the top of stack has a terminal that is <. the last popped 
 *      terminal
 *      2. Check if the popped elements match the RHS of one of the rules
 *      3. Build the abstract syntax tree after the reduction
*/
exprNode* parse_expr() {
    // Ensure stack is cleared first!
    stack.clear();

    // Peek current input token
    Token curr_input_token = peek_symbol();
    curr_input_token.Print();

    // Initialize stack with a EOE first
    stackNode eoe_node;
    eoe_node.type = TERM;
    eoe_node.term->lexeme = "$";
    eoe_node.term->token_type = END_OF_FILE;
    eoe_node.term->line_no = curr_input_token.line_no;
    stack.push_back(eoe_node);

    // Peek top terminal of stack
    stackNode curr_stack_term_top = stack_peeker();

    // If $ is on top of the stack and peek_symbol();
    while (!((curr_stack_term_top.term->token_type == END_OF_FILE) && (peek_symbol().token_type == END_OF_FILE))) {
        /**
         * curr_stack_term_top is the current term on top or just below top of stack while
         * curr_input_token is the current input token
         * 
         * In the notes, 
         * a is the token on top or just below of stack
         * t is the token from input
         * b is the token type of t 
        */
        int a = map_tokentype_indextable[curr_stack_term_top.term->token_type];
        int b = map_tokentype_indextable[curr_input_token.token_type];

        cout << a << endl;
        cout << b << endl;

        /*
        if (precedence_table[a][b] == PREC_LESS) {
            cout << "LESS!" << endl;
        } else if (precedence_table[a][b] == PREC_EQUAL) {
            cout << "EQUAL!" << endl;
        } else if (precedence_table[a][b] == PREC_GREATER) {
            cout << "GREATER!" << endl;
        } else if (precedence_table[a][b] == PREC_ERR) {
            cout << "ERROR!" << endl;
        } else if (precedence_table[a][b] == PREC_ACCEPT) {
            cout << "ACCEPT!" << endl;
        } else {
            cout << "UNKNOWN!" << endl;
        }
        */

        if ((precedence_table[a][b] == PREC_LESS) || (precedence_table[a][b] == PREC_EQUAL)) {
            // Shift
            
            // Get token from input first
            Token t = get_symbol();
            // t.Print();

            // Build the stack node from token
            stackNode t_stack_node;
            t_stack_node.type = TERM;
            t_stack_node.term->lexeme = t.lexeme;
            t_stack_node.term->line_no = t.line_no;
            t_stack_node.term->token_type = t.token_type;

            // Push to stack
            stack.push_back(t_stack_node);
        } else if (precedence_table[a][b] == PREC_GREATER) {
            // Reduce
            
            // Stack to store all the stackNode from the current RHS
            vector<stackNode> curr_rhs;

            // Store last pop term and the current top of stack
            Token* last_popped_term = new Token();
            stackNode curr_top;

            // Pop until the top of the stack is a term and the operator precedence become less than
            do {
                // Peek the top of stack
                curr_top = stack_peeker_top();

                // Change the last popped token
                if (curr_top.type == TERM) {
                    duplicate_token(curr_top, *last_popped_term);
                }
                
                // Push stackNode into RHS
                stackNode newTop;
                duplicate_stack_node(curr_top, newTop);
                curr_rhs.push_back(curr_top);

                // pop_back just delete last element without returning it
                stack.pop_back();
            }
            while (!(stack_peeker_top().type == TERM && precedence_table[stack_peeker().term->token_type][last_popped_term->token_type] == PREC_LESS));

            // Get the RHS string
            string curr_rhs_str = reverse_rhs_builder(curr_rhs);
            // cout << curr_rhs_str << endl;
            
            // RHS calculated above
            if (rhs_match(curr_rhs_str)) {
                // Determine the type of operator
                operatorType curr_operator = operator_type(curr_rhs_str);

                // We can think of curr_expr as the root of subtree for curr_expr -> RHS
                exprNode* curr_expr;

                auto curr_rhs_it = curr_rhs.end();

                // Reduction part is done below

                // Determine the kind of exprNode to build
                if (curr_operator == ID_OPER || curr_operator == NUM_OPER) {
                    // ID_OPER OR NUM_OPER
                    exprNodeType curr_expr_type = expr_type(curr_rhs_it->term->lexeme);

                    curr_expr = new exprNode(curr_operator, curr_expr_type, curr_rhs_it->term->lexeme, curr_rhs_it->term->line_no);
                } else if (curr_operator == WHOLE_ARRAY_OPER) {
                    // WHOLE_ARRAY_OPER
                    exprNodeType left_child_type = curr_rhs_it->expr->type;
                    exprNode* left_child = curr_rhs_it->expr;

                    curr_expr = new exprNode(curr_operator, left_child_type, left_child, nullptr);
                } else {
                    // PLUS_OPER, MINUS_OPER, DIV_OPER, MULT_OPET OR ARRAY_ELEM_OPER
                    
                    // First expression
                    exprNodeType left_child_type = curr_rhs_it->expr->type;
                    exprNode* left_child = curr_rhs_it->expr;

                    // Second expression
                    curr_rhs_it -= 2;
                    exprNodeType right_child_type = curr_rhs_it->expr->type;
                    exprNode* right_child = curr_rhs_it->expr;

                    // Type of new node
                    if (right_child_type != left_child_type) {
                        curr_expr = new exprNode(curr_operator, ERROR_TYPE, left_child, right_child);
                    } else {
                        curr_expr = new exprNode(curr_operator, left_child_type, left_child, right_child);
                    }
                }

                // Create stack node to house the current expression
                stackNode* stack_node_expr = new stackNode();
                stack_node_expr->type = EXPR;
                stack_node_expr->expr = curr_expr;

                // Push the expression built onto stack
                stack.push_back(*stack_node_expr);
            } else {
                syntax_error();
            }
        } else {
            syntax_error();
        }

        curr_stack_term_top = stack_peeker();
        curr_input_token = peek_symbol();
    }

    // Return the root node of expression
    return stack.at(1).expr;
}

exprNode* parse_variable_access() {
    // Make an expr Node
    Token id_token = expect(ID);
    exprNode* id_expr_node = new exprNode(ID_OPER, expr_type(id_token.lexeme), id_token.lexeme, id_token.line_no);
    
    Token t1 = lexer.peek(1);
    Token t2 = lexer.peek(2);

    if (t1.token_type == LBRAC && t2.token_type == DOT) {
        // Array access with [.]
        expect(LBRAC);
        expect(DOT);
        expect(RBRAC);

        exprNode* whole_arr_oper_expr = new exprNode(WHOLE_ARRAY_OPER, ARRAY_TYPE, id_expr_node);
        return whole_arr_oper_expr;
    } else if (t1.token_type == LBRAC) {
        // Array access [expr]
        expect(LBRAC);
        exprNode* expr_node = parse_expr();
        expect(RBRAC);

        exprNode* arr_element_oper_expr = new exprNode(ARRAY_ELEM_OPER, ARRAY_TYPE, id_expr_node, expr_node);
        return arr_element_oper_expr;
    }

    // If it is just an ID exprNode
    return id_expr_node;
}

void parse_output_stmt() {
    expect(OUTPUT);
    exprNode* outputexpr = parse_variable_access();
    expect(SEMICOLON);
}

void parse_assign_stmt() {
    exprNode* left_child_expr = parse_variable_access();

    // at this point we have parsed the [.] or [expr]
    // or we are dealing with the case ID =
    // in all cases, the next token must be EQUAL
    expect(EQUAL);
    exprNode* right_child_expr = parse_expr();
    expect(SEMICOLON);

    root = new exprNode(EQUAL_OPER, left_child_expr->type, left_child_expr, right_child_expr);
}

void parse_stmt_list() {
    // Parse Statements 1 statement at a time
    while (peek_symbol().token_type != RBRACE) {
        // Decide if it is an output or an assign statement
        if (peek_symbol().token_type == OUTPUT) {
            parse_output_stmt();
        } else {
            parse_assign_stmt();
        }
    }
}

void parse_block() {
    expect(LBRACE);
    parse_stmt_list();
    expect(RBRACE);
}

void parse_scalar() {
    expect(SCALAR);

    while (peek_symbol().token_type != ARRAY) {
        Token curr_var = expect(ID);
        scalar_list.push_back(curr_var);
    }
}

void parse_array() {
    expect(ARRAY);

    while (peek_symbol().token_type != LBRACE) {
        Token curr_var = expect(ID);
        array_list.push_back(curr_var);
    }
}

void expr_node_printer(exprNode* curr_expr) {
    operatorType curr_operator_type = curr_expr->curr_operator;
    // first_ast
}

/**
 * Prints the abstract syntax tree using BFS
*/
void print_abstract_syntax_tree() {
    queue<exprNode*> q;
    q.push(root);

    while (q.empty() == false) {
      exprNode *curr_expr = q.front();
      expr_node_printer(curr_expr);
      q.pop();
      
      if (curr_expr->child.left != NULL)
         q.push(curr_expr->child.left);
      if (curr_expr->child.right != NULL)
         q.push(curr_expr->child.right);
   }
}

// Task 1
void parse_and_generate_AST() {
    // Initialization
    initialize_map();
    
    // SCALAR and ARRAY Declaration Parsing
    parse_scalar();
    parse_array();

    // Statement Parsing
    parse_block();

    // End of statement parsing
    expect(END_OF_FILE);

    // Only if no error then you can print the ast of first statement
    // print_abstract_syntax_tree();
}

// Task 2
void parse_and_type_check() {
	cout << "2" << endl;
}

// Task 3
instNode* parse_and_generate_statement_list() {
    cout << "3" << endl;

    // The following is the hardcoded statement list 
    // generated for a specific program
    // you should replace this code with code that parses the
    // input and generayes a statement list
    // 
    // program
    // SCALAR a b c d
    // ARRAY x y z
    // a = 1;
    // b = 2;
    // c = 3;
    // d = (a+b)*(b+c);
    // OUTPUT d;
    // x[a+b] = d;
    // OUTPUT x[3];
    //
    //  a will be at location 0
    //  b will be at location 1
    //  c will be at location 2
    //  d will be at location 3
    //  x will be at location 4 - 13
    //  y will be at location 14 - 23
    //  z will be at location 24 - 33
    //  t1 will be at location 34 : intermediate value for (a+b)
    //  t2 will be at location 35 : intermediate value for (b+c)
    //  t3 will be at location 36 : intermediate value (a+b)*(b+c)
    //  t4 will be at location 37 : intermediate value for a+b index of array
    //  t5 will be at location 38 : intermediate value for addr of x[a+b] =
    //                              address_of_x + value of a+b =
    //                              4 + value of a+b
    //  t6 will be at location 39 : intermediate value for addr of x[3] =
    //                              address_of_x + value of 3 =
    //                              4 + value of 3 (computation is not done at
    //                              compile time)
    //
    instNode * i01 = new instNode();
    i01->lhsat=DIRECT; i01->lhs = 0;    // a
    i01->iType=ASSIGN_INST;             // =
    i01->op1at=IMMEDIATE; i01->op1 = 1; // 1
    i01->oper = OP_NOOP;                // no operator

    instNode * i02 = new instNode();
    i02->lhsat=DIRECT; i02->lhs = 1;    // b
    i02->iType=ASSIGN_INST;             // =
    i02->op1at=IMMEDIATE; i02->op1 = 2; // 2
    i02->oper = OP_NOOP;                // no operator

    i01->next = i02;

    instNode * i03 = new instNode();
    i03->lhsat=DIRECT; i03->lhs = 2;    // c
    i03->iType=ASSIGN_INST;             // =
    i03->op1at=IMMEDIATE; i03->op1 = 3; // 3
    i03->oper = OP_NOOP;                // no operator

    i02->next = i03;

    instNode * i1 = new instNode();
    i1->lhsat=DIRECT; i1->lhs = 34; // t1
    i1->iType=ASSIGN_INST;          // =
    i1->op1at=DIRECT; i1->op1 = 0;  // a
    i1->oper = OP_PLUS;             // +
    i1->op2at=DIRECT; i1->op2 = 1;  // b

    i03->next = i1;

    instNode * i2 = new instNode();
    i2->lhsat=DIRECT; i2->lhs = 35; // t2
    i2->iType=ASSIGN_INST;          // =
    i2->op1at=DIRECT; i2->op1 = 1;  // b
    i2->oper = OP_PLUS;             // +
    i2->op2at=DIRECT; i2->op2 = 2;  // c

    i1->next = i2;

    instNode * i3 = new instNode();
    i3->lhsat=DIRECT; i3->lhs = 36; // t3
    i3->iType=ASSIGN_INST;          // =
    i3->op1at=DIRECT; i3->op1 = 34;  // t1
    i3->oper = OP_MULT;             // *
    i3->op2at=DIRECT; i3->op2 = 35;  // t2

    i2->next = i3;                  // i3 should be after i1 and i2

    instNode * i4 = new instNode();
    i4->lhsat=DIRECT; i4->lhs = 3;  // d
    i4->iType=ASSIGN_INST;          // =
    i4->op1at=DIRECT; i4->op1 = 36; // t3
    i4->oper = OP_NOOP;             // no operator

    i3->next = i4;

    instNode * i5 = new instNode();
    i5->iType=OUTPUT_INST;          // OUTPUT
    i5->op1at=DIRECT; i5->op1 = 3;  // d

    i4->next = i5;

    instNode * i6 = new instNode();
    i6->lhsat=DIRECT; i6->lhs = 37; // t4
    i6->iType=ASSIGN_INST;          // =
    i6->op1at=DIRECT; i6->op1 = 0;  // a
    i6->oper = OP_PLUS;             // +
    i6->op2at=DIRECT; i6->op2 = 1;  // b
    i5->next = i6;

    instNode * i7 = new instNode();
    i7->lhsat=DIRECT; i7->lhs = 38;    // t5
    i7->iType=ASSIGN_INST;             // =
    i7->op1at=IMMEDIATE; i7->op1 = 4;  // address of x = 4 available
                                       // at compile time
    i7->oper = OP_PLUS;                // +
    i7->op2at=DIRECT; i7->op2 = 37;    // t5 (contains value of a+b

    i6->next = i7;

    instNode * i8 = new instNode();
    i8->lhsat=INDIRECT; i8->lhs = 38;  // x[a+b]
    i8->iType=ASSIGN_INST;             // =
    i8->op1at=DIRECT; i8->op1 = 3;     // d
    i8->oper = OP_NOOP;

    i7->next = i8;

    instNode * i9 = new instNode();
    i9->lhsat=DIRECT; i9->lhs = 39;    // t6 will contain address of x[3]
    i9->iType=ASSIGN_INST;             // =
    i9->op1at=IMMEDIATE; i9->op1 = 4;  // address of x = 4 available
                                       // at compile time
    i9->oper = OP_PLUS;                // +
    i9->op2at=IMMEDIATE; i9->op2 = 3;  // 3

    i8->next = i9;

    instNode * i10 = new instNode();
    i10->iType=OUTPUT_INST;              // OUTPUT
    i10->op1at=INDIRECT; i10->op1 = 39;  // x[3] by providing its
                                         // address indirectly through
                                         // t6

    i9->next = i10;

    instNode* code = i01;

    return code;
}
