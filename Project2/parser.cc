#include <queue>
#include <sstream>
#include <typeinfo>
#include <unordered_map>
#include "error.h"
#include "parser.h"

using namespace std;


// First statement AST
stringstream first_ast;

LexicalAnalyzer lexer;
vector<exprNode*> stack_main;
vector<stackNode*> stack_statement_parsing;

unordered_map<TokenType, int> map_tokentype_indextable;
unordered_set<string> valid_rhs;

unordered_set<string> scalar_list;
unordered_set<string> array_list;

stringstream expression_type_error_string;
stringstream assignment_error_string;

bool expression_type_error_trigger = false;
bool assignment_error_trigger = false;

precedenceValue precedence_table[12][12] = {
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
    {PREC_LESS, PREC_LESS, PREC_LESS, PREC_LESS, PREC_LESS, PREC_ERR, PREC_LESS, PREC_ERR, PREC_ERR, PREC_LESS, PREC_LESS, PREC_ACCEPT}
};

// For printing the BFS tree
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

// If the RHS is none of this, then it will be syntax error
// But we will also need to store if it is unary or binary 
// and the position of expr 
void intialize_rhs() {
    valid_rhs.insert("E-E");
    valid_rhs.insert("E+E");
    valid_rhs.insert("E*E");
    valid_rhs.insert("E/E");
    valid_rhs.insert("(E)");
    valid_rhs.insert("E[E]");
    valid_rhs.insert("E[.]");
    valid_rhs.insert("ID");
    valid_rhs.insert("NUM");
}

// Return the type of operator the expression string that is reduced
operatorType operator_type(string curr_rhs) {
    if (curr_rhs == "ID") {
        return ID_OPER;
    } else if (curr_rhs == "(E)") {
        return EXPR_OPER;
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

Token get_symbol() {
    return lexer.GetToken();
}

Token peek_symbol(variableAccessType access_type) {
    // 3 cases of EOE
    // If the next token is SEMICOLON
    // If the next token is RBRAC and the token after that is EQUAL
    // If the next token is RBRAC and the token after that is SEMICOLON

    Token t1 = lexer.peek(1);
    Token t2 = lexer.peek(2);

    if (t1.token_type == SEMICOLON || (t1.token_type == RBRAC && t2.token_type == EQUAL)) {
        Token eoe_tok;
        eoe_tok.lexeme = "$";
        eoe_tok.token_type = END_OF_FILE;
        eoe_tok.line_no = t1.line_no;

        return eoe_tok;
    } else {
        return t1;
    }
}

// Prints the expression node based on its type and its lexeme
void expr_node_printer(exprNode* curr_expr) {
    operatorType curr_operator_type = curr_expr->curr_operator;

    if (curr_operator_type == ID_OPER || curr_operator_type == NUM_OPER) {
        first_ast << expr_string[(int)curr_operator_type] + "\"";
        first_ast << curr_expr->id.varName;
        first_ast << "\" ";
    } else {
        first_ast << expr_string[(int)curr_operator_type] + " ";
    }
}

/**
 * Prints the abstract syntax tree using BFS
*/
void print_abstract_syntax_tree() {
    exprNode* root = stack_main[0];

    if (root == nullptr) {
        return;
    }

    queue<exprNode*> bfs_queue;
    bfs_queue.push(root);

    while (!bfs_queue.empty()) {
      exprNode *curr_expr = bfs_queue.front();
      expr_node_printer(curr_expr);
      bfs_queue.pop();

      if (curr_expr->child.left != nullptr)
         bfs_queue.push(curr_expr->child.left);
      if (curr_expr->child.right != nullptr)
         bfs_queue.push(curr_expr->child.right);
   }

   cout << first_ast.str() << endl;
}

/**
 * Returns the Token of closest to the top of stack or just below top of stack
*/
Token terminal_peek(vector<stackNode*> curr_stack) {
    int stack_len = curr_stack.size();

    if (curr_stack[stack_len - 1]->type == TERM) {
        return curr_stack[stack_len - 1]->term;
    } else {
        return curr_stack[stack_len - 2]->term;
    }
}

Token expect(TokenType expected_type) {
    Token curr_token = get_symbol();

    if (curr_token.token_type != expected_type)
        syntax_error();

    return curr_token;
}

// Reverses the stackNodes built from the RHS Stack and return a string of RHS
string reverse_rhs_builder(vector<stackNode*> rhs_stack) {
    stringstream rhs_string_stream;
    int rhs_stack_size = rhs_stack.size();

    for (int ii = rhs_stack_size - 1; ii >= 0; ii--) {
        if (rhs_stack[ii]->type == TERM) {
            if (rhs_stack[ii]->term.token_type == PLUS) {
                string expr_str = "+";
                rhs_string_stream << expr_str;
            } else if (rhs_stack[ii]->term.token_type == MINUS) {
                string expr_str = "-";
                rhs_string_stream << expr_str;
            } else if (rhs_stack[ii]->term.token_type == MULT) {
                string expr_str = "*";
                rhs_string_stream << expr_str;
            } else if (rhs_stack[ii]->term.token_type == DIV) {
                string expr_str = "/";
                rhs_string_stream << expr_str;
            } else if (rhs_stack[ii]->term.token_type == LPAREN) {
                string expr_str = "(";
                rhs_string_stream << expr_str;
            } else if (rhs_stack[ii]->term.token_type == RPAREN) {
                string expr_str = ")";
                rhs_string_stream << expr_str;
            } else if (rhs_stack[ii]->term.token_type == LBRAC) {
                string expr_str = "[";
                rhs_string_stream << expr_str;
            } else if (rhs_stack[ii]->term.token_type == RBRAC) {
                string expr_str = "]";
                rhs_string_stream << expr_str;
            } else if (rhs_stack[ii]->term.token_type == ID) {
                string expr_str = "ID";
                rhs_string_stream << expr_str;
            } else if (rhs_stack[ii]->term.token_type == NUM) {
                string expr_str = "NUM";
                rhs_string_stream << expr_str;
            } else if (rhs_stack[ii]->term.token_type == DOT) {
                string expr_str = ".";
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
exprNode* parse_expr(variableAccessType access_type) {
    // Ensure stack is cleared first!
    stack_statement_parsing.clear();

    // Initialize stack with a EOE first
    stackNode* eoe_stacknode = new stackNode();
    eoe_stacknode->type = TERM;
    eoe_stacknode->term.token_type = END_OF_FILE;
    stack_statement_parsing.push_back(eoe_stacknode);

    // If $ is on top of the stack and peek_symbol() is also $;
    while (!(terminal_peek(stack_statement_parsing).token_type == END_OF_FILE && peek_symbol(access_type).token_type == END_OF_FILE)) {
        /**
         * curr_stack_term_top is the current term on top or just below top of stack while
         * curr_input_token is the current input token
         * 
         * In the notes, 
         * a is the token on top or just below of stack
         * t is the token from input
         * b is the token type of t 
        */
        Token t = peek_symbol(access_type);
        TokenType token_type_input = t.token_type;

        // Check if the input token type can be found in the precedence table
        if (map_tokentype_indextable.find(token_type_input) == map_tokentype_indextable.end()) {
            syntax_error();
        }

        int a = map_tokentype_indextable[terminal_peek(stack_statement_parsing).token_type];
        int b = map_tokentype_indextable[token_type_input];
        
        // cout << a << endl;
        // cout << b << endl;

        if ((precedence_table[a][b] == PREC_LESS) || (precedence_table[a][b] == PREC_EQUAL)) {
            // Shift

            // Get token from input first
            t = get_symbol();

            // Build the stack node from token
            // You need to allocate memory for the term pointer otherwise 
            // it will seg fault if you directly assign it with values 
            stackNode* t_stack_node = new stackNode();
            t_stack_node->type = TERM;
            t_stack_node->term = t;
            // t_stack_node->term.Print();

            // Push to stack
            stack_statement_parsing.push_back(t_stack_node);
        } else if (precedence_table[a][b] == PREC_GREATER) {
            // Reduce

            // Stack to store all the stackNode from the current RHS
            vector<stackNode*> curr_rhs;

            // Store last pop term
            stackNode* last_popped_term = new stackNode();

            // Pop until the top of the stack is a term and the operator precedence become less than
            do {
                // Peek the top of stack
                int curr_stmt_stack_size = stack_statement_parsing.size();
                stackNode* top_stacknode = stack_statement_parsing[curr_stmt_stack_size - 1];
                
                if (top_stacknode->type == TERM) {
                    last_popped_term = top_stacknode;
                }

                // Push stackNode into RHS
                curr_rhs.push_back(top_stacknode);

                // pop_back just delete last element without returning it
                stack_statement_parsing.pop_back();
            } while(!(stack_statement_parsing[stack_statement_parsing.size() - 1]->type == TERM && precedence_table[map_tokentype_indextable[terminal_peek(stack_statement_parsing).token_type]][map_tokentype_indextable[last_popped_term->term.token_type]] == PREC_LESS));

            // Get the RHS string
            string rhs_string = reverse_rhs_builder(curr_rhs);
            // cout << rhs_string << endl;

            // Check if the string is indeed valid
            if (valid_rhs.find(rhs_string) != valid_rhs.end()) {
                // root_stacknode is the root of subtree for root_stacknode -> RHS
                stackNode* root_stacknode = new stackNode();
                root_stacknode->type = EXPR;

                // Determine the type of operator
                operatorType curr_operator = operator_type(rhs_string);

                // Reduction part is done below
                if (curr_operator == ID_OPER || curr_operator == NUM_OPER) {
                    // ID and NUM is just itself being the root
                    root_stacknode->expr = new exprNode();
                    root_stacknode->expr->curr_operator = curr_operator;
                    root_stacknode->expr->id.line_no = curr_rhs[0]->term.line_no;
                    root_stacknode->expr->id.varName = curr_rhs[0]->term.lexeme;

                    // No child
                    root_stacknode->expr->child.right = nullptr;
                    root_stacknode->expr->child.left = nullptr;
					
					if (curr_operator == ID_OPER) {
                        // Type check
						if (scalar_list.find(curr_rhs[0]->term.lexeme)!=scalar_list.end()) {
							root_stacknode->expr->type = SCALAR_TYPE;
						} else if(array_list.find(curr_rhs[0]->term.lexeme)!=array_list.end()) {
							root_stacknode->expr->type = ARRAYDDECL_TYPE;
						} else {
							root_stacknode->expr->type = ERROR_TYPE;
						}
					} else {
                        root_stacknode->expr->type = SCALAR_TYPE;
                    }

                } else if (curr_operator == WHOLE_ARRAY_OPER) {
                    root_stacknode->expr = new exprNode();
                    root_stacknode->expr->curr_operator = curr_operator;

                    // Only 1 expression
                    stackNode* left_child = new stackNode();
                    left_child = curr_rhs[3];
                    root_stacknode->expr->child.left = left_child->expr;

                    // Set the rootnode's line number 
                    root_stacknode->expr->id.line_no = left_child->expr->id.line_no;

                    // No right child!
                    root_stacknode->expr->child.right = nullptr;

                    // Type check
                    if (root_stacknode->expr->curr_operator == NUM_OPER) {
                        root_stacknode->expr->type = ARRAY_TYPE;
                    } else if (left_child->expr->type != ARRAYDDECL_TYPE) {
                        root_stacknode->expr->type = ERROR_TYPE;
                    } else {
                        root_stacknode->expr->type = ARRAY_TYPE;
                    }

                } else if (curr_operator == EXPR_OPER) {
                    root_stacknode = curr_rhs[1];
                    root_stacknode->expr->type = curr_rhs[1]->expr->type;
                    root_stacknode->expr->id.line_no = curr_rhs[1]->expr->id.line_no;

                } else if (curr_operator == ARRAY_ELEM_OPER) {
                    // ARRAY_ELEM_OPER
                    root_stacknode->expr = new exprNode();
                    root_stacknode->expr->curr_operator = ARRAY_ELEM_OPER;

                    // First expression
                    stackNode* left_child= new stackNode();
                    left_child = curr_rhs[3];
                    root_stacknode->expr->child.left = left_child->expr;

                    // Set the rootnode's line number
                    root_stacknode->expr->id.line_no = left_child->expr->id.line_no;

                    // Second expression
                    stackNode* right_child = new stackNode();
                    right_child = curr_rhs[1];
                    root_stacknode->expr->child.right = right_child->expr;

                    // Type check
                    if (left_child->expr->type != ARRAYDDECL_TYPE) {
                        root_stacknode->expr->type = ERROR_TYPE;
                    } else if(right_child->expr->type != SCALAR_TYPE) {
                        root_stacknode->expr->type = ERROR_TYPE;
                    } else {
                        root_stacknode->expr->type = SCALAR_TYPE;
                    }

                } else {
                    // PLUS_OPER, MINUS_OPER, DIV_OPER, MULT_OPER
                    root_stacknode->expr = new exprNode();
                    root_stacknode->expr->curr_operator = curr_operator;

                    // First expression
                    stackNode* left_child =new stackNode();
                    left_child = curr_rhs[2];
                    root_stacknode->expr->child.left = left_child->expr;

                    // Set the rootnode's line number
                    root_stacknode->expr->id.line_no = left_child->expr->id.line_no;

                    // Second expression
                    stackNode* right_child = new stackNode();
                    right_child = curr_rhs[0];
                    root_stacknode->expr->child.right = right_child->expr;

                    if (left_child->type == 0 && right_child->type == 0) {
                        root_stacknode->expr->type = SCALAR_TYPE;
                    } else if(left_child->type == 2 && right_child->type == 2) {
                        root_stacknode->expr->type = ARRAY_TYPE;
                    } else {
                        root_stacknode->expr->type = ERROR_TYPE;
                    }
                }

                // Push the expression built onto stack
                stack_statement_parsing.push_back(root_stacknode);
            } else {
                syntax_error();
            }
        } else {
            syntax_error();
        }
    }

    return stack_statement_parsing[1]->expr;
}       

exprNode* parse_variable_access(variableAccessType access_type) {
    Token id_token = expect(ID);

    // ID exprNode
    exprNode* id_exprnode = new exprNode();
    id_exprnode->curr_operator = ID_OPER;
    id_exprnode->id.line_no = id_token.line_no;
    id_exprnode->id.varName = id_token.lexeme;

    // ID has no child
    id_exprnode->child.left = nullptr;
    id_exprnode->child.right = nullptr;

    Token t1 = lexer.peek(1);
    Token t2 = lexer.peek(2);

    exprNode* root_exprnode = new exprNode();
    root_exprnode->id.line_no = id_token.line_no;

    if (t1.token_type == SEMICOLON || t1.token_type == EQUAL) {
        /**
         * If the assignment is either ID itself
        */ 

        // Check for type
        if (array_list.find(id_token.lexeme) != array_list.end()) {
            id_exprnode->type = ARRAYDDECL_TYPE;
        } else if(scalar_list.find(id_token.lexeme) != scalar_list.end()) {
            id_exprnode->type = SCALAR_TYPE;
        } else {
            id_exprnode->type = ERROR_TYPE;
        }

        // id; OR id = expr;
        return id_exprnode;
    } else if (t1.token_type == LBRAC) {
        /**
         * If there is array access with [.] or [expr]
        */
        if (t2.token_type == DOT) {
            // Array access with [.]
            expect(LBRAC);
            expect(DOT);
            expect(RBRAC);

            // Building the id[.] = expr
            root_exprnode->child.left = id_exprnode;
            root_exprnode->curr_operator = WHOLE_ARRAY_OPER;
            root_exprnode->child.right = nullptr;

            // Check for type
            if (id_exprnode->type == ERROR_TYPE) {
                root_exprnode->type = ERROR_TYPE;
            } else if (id_exprnode->type == SCALAR_TYPE) {
                root_exprnode->type = ERROR_TYPE;
            } else if (id_exprnode->type == ARRAYDDECL_TYPE) {
                root_exprnode->type = ARRAY_TYPE;
            } else {
                root_exprnode->type = ERROR_TYPE;
            }
        } else {
            // Array access [expr]
            expect(LBRAC);
            exprNode* right_child = new exprNode();
            right_child = parse_expr(access_type);
            expect(RBRAC);

            // Building the id[expr] = expr
            root_exprnode->child.left = id_exprnode;
            root_exprnode->curr_operator = ARRAY_ELEM_OPER;
            root_exprnode->child.right = right_child;

            // Check for type
            if (id_exprnode->type != ARRAYDDECL_TYPE) {
                root_exprnode->type = ERROR_TYPE;
            } else if (right_child->type != SCALAR_TYPE) {
                root_exprnode->type = ERROR_TYPE;
            } else {
                root_exprnode->type = SCALAR_TYPE;
            }
        }
    } else {
        syntax_error();
    }

    return root_exprnode;   
}

exprNode* parse_output_stmt() {
    expect(OUTPUT);
    exprNode* head = new exprNode();
    head = parse_variable_access(OUTPUT_ACCESS);

    expect(SEMICOLON);
    return head;
}

exprNode* parse_assign_stmt() {
    // Left child comes from parsing of variable access
    exprNode* left_child = new exprNode();
    left_child = parse_variable_access(ASSIGN_ACCESS);
    expect(EQUAL);

    // at this point we have parsed the [.] or [expr]
    // or we are dealing with the case ID =
    // in all cases, the next token must be EQUAL

    // Right child comes from the parse expresssion
    exprNode* right_child = new exprNode();
    right_child = parse_expr(ASSIGN_ACCESS);
    expect(SEMICOLON);

    // Root node consist of equal and the left child from 
    // parse_variable_access and right child from parse_expr
    exprNode* root_exprnode = new exprNode();
    root_exprnode->curr_operator = EQUAL_OPER;
    root_exprnode->id.line_no = right_child->id.line_no;
    root_exprnode->child.left = left_child;
    root_exprnode->child.right = right_child;

    // Expression type for left and right child
    exprNodeType left_child_type = left_child->type;
    exprNodeType right_child_type = right_child->type;
    
    // Type Checking
    if (left_child_type == ERROR_TYPE || right_child_type == ERROR_TYPE || left_child_type == ARRAYDDECL_TYPE || right_child_type == ARRAYDDECL_TYPE) {
        expression_type_error_trigger = true;
        string curr_line_expression_type_error = "\nLine " + to_string(root_exprnode->id.line_no);
        expression_type_error_string << curr_line_expression_type_error;
    }

    if (!(left_child_type == ARRAY_TYPE || right_child_type == SCALAR_TYPE)) {
        assignment_error_trigger = true;
        string curr_line_assignment_error = "\nLine " + to_string(root_exprnode->id.line_no);
        assignment_error_string << curr_line_assignment_error;
    }
    
    return root_exprnode;
}

void parse_stmt() {
    Token t = lexer.peek(1);

    if (t.token_type == ID) {
        exprNode* root_exprnode = new exprNode();
        root_exprnode = parse_assign_stmt();
        stack_main.push_back(root_exprnode);
    } else if(t.token_type == OUTPUT) {
        exprNode* root_exprnode = new exprNode();
        root_exprnode = parse_output_stmt();
        stack_main.push_back(root_exprnode);
    } else {
        syntax_error();
    }
}

void parse_stmt_list() {
    while (lexer.peek(1).token_type != RBRACE) {
        parse_stmt();
    }
}

void parse_block() {
    expect(LBRACE);
    parse_stmt_list();
    expect(RBRACE);
}

void parse_id_list() {
    // Scalar declaration
    expect(SCALAR);    
    while (lexer.peek(1).token_type != ARRAY) {
        Token curr_var = expect(ID);
        scalar_list.insert(curr_var.lexeme);
    }

    // Array declaration
    expect(ARRAY);
    while (lexer.peek(1).token_type != LBRACE) {
        Token curr_var = expect(ID);
        array_list.insert(curr_var.lexeme);
    }
}

void parse_decl_section() {
    parse_id_list();
}

void parse_input() {
    parse_decl_section();
    parse_block(); 
}

void parse_task_1() {
    // Intialize the RHS and index mapping for token type
    initialize_map();
    intialize_rhs();

    // Parse the input and build the AST
    parse_input();
    expect(END_OF_FILE);
}
