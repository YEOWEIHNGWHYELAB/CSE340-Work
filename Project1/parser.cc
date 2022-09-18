/**
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include <iostream>
#include <cstdlib>

#include "parser.h"
#include "mylexer.h"

using namespace std;

#define WHITE_SPACE ' '

// Global LexicalAnalyzer & myLexicalAnalyzer object
myLexicalAnalyzer lexer_reg;
LexicalAnalyzer lexer;

// Semantic error list
vector<string> sem_error_str;

// Operator overload for RegNode Comparision
static bool operator<(const RegNode& node_1, const RegNode& node_2) {
    return node_1.node_number < node_2.node_number;
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type) {
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error_general();
    return t;
}

Token Parser::expect_expr(string token_str, TokenType expected_type) {
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error_expr(token_str);
    return t;
}

/**
 * Parses a regular expression and returns the REG of the regular expression that is parsed
 */
struct REG * Parser::parse_expr() {
    /**
     * expr -> CHAR
     * expr -> UNDERSCORE
     * expr -> LPAREN
     */
    Token curr_t;
    Token after_RPAREN;
    
    // No need to use while loop to check for COMMA as that is taken 
    // care of at parse_token_list
    curr_t = lexer.GetToken(); // Consume token

    if (curr_t.token_type == CHAR || curr_t.token_type == UNDERSCORE){
        /**
         * Single Character Or it could be Just Epsilon Only 
         * 1 Start and 1 Accept Node (Final)
         * 
         * expr -> CHAR
         * expr -> UNDERSCORE
         */

        REG * current_reg = new REG();

        // For single character -> only 2 RegNode which is 
        // the first and final node itself
        RegNode * start_reg_node = new RegNode();
        RegNode * final_reg_node  = new RegNode();

        // REG for the first and end node of graph
        current_reg->start = start_reg_node;
        current_reg->accept = final_reg_node;

        start_reg_node->first_neighbor = final_reg_node;

        if (curr_t.token_type == CHAR) 
            start_reg_node->first_label = curr_t.lexeme.at(0);
        else 
            start_reg_node->first_label = '_';

        // Insert numbering to RegNode
        start_reg_node->node_number = lexer_reg.getCurrentNodeNumber();
        final_reg_node->node_number = lexer_reg.getCurrentNodeNumber();
        
        return current_reg;
    } else if (curr_t.token_type == LPAREN) {
        /**
         * For expr with LPAREN, there are only 3 possibility
         * 
         * CASE 1: expr -> LPAREN expr RPAREN STAR
         * CASE 2: expr -> LPAREN expr RPAREN DOT LPAREN expr RPAREN
         * CASE 3: expr -> LPAREN expr RPAREN OR LPAREN expr RPAREN
         */

        // Recursively call parse_expr() for any expression
        REG * expr1_reg = parse_expr();
        expect_expr(current_token_name, RPAREN);

        Token after_RPAREN = lexer.GetToken();

        if (after_RPAREN.token_type == DOT || after_RPAREN.token_type == OR){
            /**
             * For CASE 2 & 3
             * 
             * And we will recursively call parse_expr() whenever theres expression
             */
            expect_expr(current_token_name, LPAREN);
            REG * expr2_reg = parse_expr();
            expect_expr(current_token_name, RPAREN);

            if (after_RPAREN.token_type == OR){
                // CASE 3

                // New REG form by combining the existing expr1_reg and expr2_reg
                // with the new start and accept node 
                REG * new_reg = new REG();

                // Following the project instruction for OR, you need a new 
                // start_node and accept_node
                RegNode * start_node = new RegNode();
                RegNode * accept_node = new RegNode();

                // New REG will use the new start and accept node as the start 
                // and end of the new REG  
                new_reg->start = start_node;
                new_reg->accept = accept_node;
                
                // Point the accept of expr1 and expr2 to the same new accept node
                expr1_reg->accept->first_neighbor = accept_node;
                expr1_reg->accept->first_label = '_';
                expr2_reg->accept->first_neighbor = accept_node;
                expr2_reg->accept->first_label = '_';

                // Initializing start node
                start_node->first_neighbor = expr1_reg->start;
                start_node->second_neighbor = expr2_reg->start;
                start_node->first_label = '_';
                start_node->second_label = '_';
                start_node->node_number = lexer_reg.getCurrentNodeNumber();

                // Initializing accept node
                accept_node->node_number = lexer_reg.getCurrentNodeNumber();

                return new_reg;
            } else{
                // CASE 2

                // New REG to combine the expr1 REG and expr2 REG
                REG * new_reg = new REG();
                new_reg->start = expr1_reg->start;
                new_reg->accept = expr2_reg->accept;

                // Linkage between expr1 REG and expr2 REG
                expr1_reg->accept->first_neighbor = expr2_reg->start;
                expr1_reg->accept->first_label = '_';

                return new_reg;
            }
        } else if (after_RPAREN.token_type == STAR) {
            /**
             * For CASE 1
             */
            REG * new_reg = new REG();
            RegNode * start_node = new RegNode();
            RegNode * accept_node = new RegNode();

            new_reg->start = start_node;
            new_reg->accept = accept_node;

            // Initializing start node according to description for *
            start_node->first_neighbor = expr1_reg->start;
            start_node->second_neighbor = accept_node;
            start_node->first_label = '_';
            start_node->second_label = '_';
            start_node->node_number = lexer_reg.getCurrentNodeNumber();
            
            // Initializing accept node
            accept_node->node_number = lexer_reg.getCurrentNodeNumber();
            
            // Point the expr1_reg to new accept_node created and its 
            // own start node
            expr1_reg->accept->first_neighbor = accept_node;
            expr1_reg->accept->second_neighbor = expr1_reg->start;
            expr1_reg->accept->first_label = '_';
            expr1_reg->accept->second_label = '_';
            
            // Link the end of expr1 REG to the new accpet node created
            expr1_reg->accept = accept_node;

            return new_reg;
        } else {
            syntax_error_expr(current_token_name);
            
            // Useless code place here just to avoid compiler warning...
            return NULL;
        }
    } else {
        syntax_error_expr(current_token_name);

        // Useless code place here just to avoid compiler warning...
        return NULL;
    }
}

/**
 * Consume token
 */ 
void Parser::parse_token() {
    // Constructing TOKEN REG (REG + lexeme (name of token))
    TOKEN_REG current_token_reg;

    // Notice that all token begins with an ID first other than
    // the case where totally no token is given
    Token t = expect(ID);
    current_token_name = t.lexeme;
    
    // Assign line number to token_reg
    current_token_reg.line_number = t.line_no;

    // Check if token name already present
    vector<TOKEN_REG> current_tokens = lexer_reg.getTokensList();
    int token_size = current_tokens.size();

    // Check if the token has already been declared
    if (token_size > 0) {
        for (auto i = current_tokens.begin(); i != current_tokens.end(); i++) {
            if (i->token_name == t.lexeme) {
                has_sem_error = true;

                string line_number_new = to_string(t.line_no);
                string line_number_orginal = to_string(i->line_number);
                curr_episilon_error += ("Line " + line_number_new + ": " + t.lexeme + " already declared on line " + line_number_orginal);
                sem_error_str.push_back(curr_episilon_error);
                curr_episilon_error = "";

                // Consume the entire token without and then discarding it
                parse_expr();

                return;
            }
        }
    }

    // Assign token name to current TOKEN REG
    current_token_reg.token_name = t.lexeme; // In this case t.lexeme is the name of token

    // Begin constructing REG
    REG * reg = parse_expr();
        
    // Assign the constructed REG to current_token_reg
    current_token_reg.reg = reg;

    // Check for Epsilon Error
    set<RegNode> current_reg_node_set;
    current_reg_node_set.insert(*reg->start);
    set<RegNode> reachable_node_from_start = lexer_reg.match_one_char(current_reg_node_set, '_');
    current_reg_node_set.insert(reachable_node_from_start.begin(), reachable_node_from_start.end());
    if (current_reg_node_set.find(*reg->accept) != current_reg_node_set.end()) {
        has_epsilon_error = true;
        curr_episilon_error += (" " + current_token_reg.token_name);
    }

    // Assign the TOKEN REG to myLexicalAnalyzer object
    lexer_reg.addTokenToTokenList(current_token_reg);
}

/**
 * Will recursively call parse_token_list until we run out of token
 */
void Parser::parse_token_list() {
    parse_token();
    Token t = lexer.peek(1);

    if (t.token_type == COMMA) {        
        expect(COMMA);
        parse_token_list();
    } else if (t.token_type == HASH) {

        // End of token list
        return;
    } else {
        syntax_error_general();
    }
}

void Parser::parse_tokens_section() {
    // Will call parse token list to store the token list
    parse_token_list();

    // HASH is used to signal end of token 
    expect(HASH);

    // Semantic & Epsilon Check
    if (has_sem_error) {
        // Only if no syntax error
        syntax_error_sem(sem_error_str);
    } else if (has_epsilon_error) {
        // Only if no syntax error nor semantic error then
        // we will do epsilon error check
        syntax_error_epsilon(curr_episilon_error);
    }
}

/**
 * Will handle the input from the buffer
 */ 
void Parser::parse_input() {
    /**
     * Formatting input string to format to be passed to my_LexicalAnalyzer
     * 
     * Handling token by constructing REG
     */
    parse_tokens_section();

    /**
     * Formatting input string to format to be passed to my_LexicalAnalyzer
     * 
     * After parsing all token, perform parsing on INPUT_TEXT and also remember
     * that INPUT_TEXT must start with " and end with "
     */
    Token input_token = expect(INPUT_TEXT);

    int input_text_len = input_token.lexeme.size();

    // Look at the test cases, input string is given in quotation 
    // marks and there may or may not be "" 
    string input_text_without_quote = input_token.lexeme.substr(1, input_text_len - 2);

    string formatted_input_str;

    // There may or may not be white spaces at the leading and 
    // trailing edges of input text excluding quotation marks  
    if (input_text_without_quote.at(0) == WHITE_SPACE) {
        formatted_input_str = input_text_without_quote.substr(1, input_text_without_quote.size() - 1);
    } else {
        formatted_input_str = input_text_without_quote.substr(0, input_text_without_quote.size() - 1);
    }

    if (input_text_without_quote.at(input_text_without_quote.size() - 1) == WHITE_SPACE) {
        formatted_input_str = formatted_input_str.substr(0, input_text_without_quote.size() - 1);
    }

    // cout << "Raw String: ->" + input_token.lexeme + "<-" << endl;
    // cout << "Input String: ->" + formatted_input_str + "<-" << endl;

    // Parse input string
    lexer_reg.setCurrentInputString(formatted_input_str);
    lexer_reg.my_GetToken();
}

// This function simply reads and prints all tokens
// I included it as an example. You should compile the provided code
// as it is and then run ./a.out < tests/test0.txt to see what this function does
void Parser::readAndPrintAllInput() {
    Token t;

    // get a token
    t = lexer.GetToken();

    // while end of input is not reached
    while (t.token_type != END_OF_FILE) {
        t.Print(); // print token
        t = lexer.GetToken(); // and get another one
    }

    // note that you should use END_OF_FILE and not EOF
}

int main() {
    // note: the parser class has a lexer object instantiated in it (see file
    // parser.h). You should not be declaring a separate lexer object.
    // You can access the lexer object in the parser functions as shown in
    // the example  method Parser::readAndPrintAllInput()
    // If you declare another lexer object, lexical analysis will
    // not work correctly

    // Note that even if you set to other values other than zero, it will 
    // still pass the test case since it increments and this project does 
    // not require that we have the node to start with 0 
    lexer_reg.setCurrentNodeNumber(0);

    Parser parser;
    parser.parse_input();

    // parser.readAndPrintAllInput();
}
