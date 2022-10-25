#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "lexer.h"
#include "execute.h"
#include "tasks.h"
#include "error.h"

using namespace std;

LexicalAnalyzer lexer;
vector<stackNode> stack;

vector<Token> scalar_list;
vector<Token> array_list;

// Initailize the map that maps token type index to the correct index precedence table 
void initialize_map() {
    map_tokentype_indextable.insert(pair<int, int>(PLUS, 0));
    map_tokentype_indextable.insert(pair<int, int>(MINUS, 1));
    map_tokentype_indextable.insert(pair<int, int>(MULT, 2));
    map_tokentype_indextable.insert(pair<int, int>(DIV, 3));
    map_tokentype_indextable.insert(pair<int, int>(LPAREN, 4));
    map_tokentype_indextable.insert(pair<int, int>(RPAREN, 5));
    map_tokentype_indextable.insert(pair<int, int>(LBRAC, 6));
    map_tokentype_indextable.insert(pair<int, int>(DOT, 7));
    map_tokentype_indextable.insert(pair<int, int>(RBRAC, 8));
    map_tokentype_indextable.insert(pair<int, int>(NUM, 9));
    map_tokentype_indextable.insert(pair<int, int>(ID, 10));
    map_tokentype_indextable.insert(pair<int, int>(END_OF_FILE, 11));
}

// Returns a END_OF_EXPRESSION Token
Token eoe_token(int line_num) {
    Token *eoe_token = new Token();
    eoe_token->lexeme = "$";
    eoe_token->line_no = line_num;
    eoe_token->token_type = END_OF_FILE;

    return *eoe_token;
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
        return eoe_token(next_token_t1.line_no);
    } else if (next_token_t1.token_type == RBRAC) {
        Token next_token_t2 = lexer.peek(2);

        if (next_token_t2.token_type == EQUAL || next_token_t2.token_type == SEMICOLON) {
            return eoe_token(next_token_t1.line_no);
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
        return *stack_peeker;
    } else {
        // If top is not a TERM, peek below by one on the stack 
        stack_peeker -= 1;

        // Check if the element below the top of the stack is stack
        if (stack_peeker->type == TERM) {
            return *stack_peeker;
        } else {
            syntax_error();
        }
    }
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

// Check if the current rhs to match is valid
bool rhs_match(string curr_rhs) {

}

/**
 * Performs operator precedence parsing
 * 
 * There are 2 possible action -> Reduce or Shift
*/
void operator_precedence_parsing(stackNode curr_stack_term_top, Token curr_input_token) {
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

    if ((precedence_table[a][b] == PREC_LESS) || (precedence_table[a][b] == PREC_EQUAL)) {
        // Shift

        // Get token from input first
        Token t = get_symbol();

        // Build the stack node from token
        stackNode t_stack_node;
        t_stack_node.type = TERM;
        t_stack_node.term = &t; 

        // Push to stack
        stack.push_back(t_stack_node);
    } else if (precedence_table[a][b] == PREC_GREATER) {
        // Reduce
        
        // Stack to store all the stackNode from the current RHS
        vector<stackNode> curr_rhs;

        // Store the last pop term
        Token* last_popped_term = new Token();

        // Peek the top of stack
        stackNode curr_top = stack_peeker_top();

        // Get the first last popped token
        duplicate_token(curr_top, *last_popped_term);
        
        // Pop until the top of the stack is a term and the operator precedence become less than
        while (curr_top.type == TERM && precedence_table[stack_peeker().term->token_type][last_popped_term->token_type] == PREC_LESS) {
            if (curr_top.type == TERM) {
                last_popped_term = curr_top.term;
            }
            
            curr_rhs.push_back(curr_top);
            curr_top = stack_peeker_top();

            stack.pop_back();
        }
        
        // RHS calculated above
        if (E -> RHS rule exists) {
            // we can think of E as the root of subtree for E -> RHS
            reduce E -> RHS
            stack.push_back(E);
        } else {
            syntax_error();
        }
    } else {
        syntax_error();
    }
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
    // Peek top terminal of stack
    stackNode curr_stack_term_top = stack_peeker();

    // Peek current input token
    Token curr_input_token = peek_symbol();

    // If $ is on top of the stack and lexer.peek() = $
    while ((curr_input_token.token_type == END_OF_FILE) && (curr_stack_term_top.term->token_type == END_OF_FILE)) {
        operator_precedence_parsing(curr_stack_term_top, curr_input_token);

        curr_stack_term_top = stack_peeker();
        curr_input_token = peek_symbol();
    }

    return new exprNode();
}

void parse_assign_stmt() {
    expect(ID);
    Token t1 = lexer.peek(1);
    Token t2 = lexer.peek(2);

    cout << t1.lexeme << endl;
    cout << t2.lexeme << endl;
    
    if (t1.token_type == LBRAC && t2.token_type == DOT) {
        // array access with .
        expect(LBRAC);
        expect(DOT);
        expect(RBRAC);
    } else if (t1.token_type == LBRAC) {
        // array access but not .
        expect(LBRAC);
        parse_expr();
        expect(RBRAC);
    }

    // at this point we have parsed the [.] or [expr]
    // or we are dealing with the case ID =
    // in all cases, the next token must be EQUAL
    expect(EQUAL);
    parse_expr();
    expect(SEMICOLON);
}

void parse_block() {
    expect(LBRACE);

    // Parse Statements 1 statement at a time
    while (lexer.peek(1).token_type != RBRAC) {
        parse_assign_stmt();
    }

    expect(RBRACE);
}

void parse_scalar() {
    expect(SCALAR);

    while (lexer.peek(1).token_type != ARRAY) {
        Token curr_var = expect(ID);
        scalar_list.push_back(curr_var);
    }
}

void parse_array() {
    expect(ARRAY);

    while (lexer.peek(1).token_type != LBRACE) {
        Token curr_var = expect(ID);
        scalar_list.push_back(curr_var);
    }
}

/**
 * Prints the abstract syntax tree using BFS
*/
void print_abstract_syntax_tree() {

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

    // Perform BFS printing here (only if every statement has no syntax error)
    print_abstract_syntax_tree();
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
