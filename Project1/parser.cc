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

using namespace std;

#define WHITE_SPACE ' '

void Parser::syntax_error_null()
{
    cout << "SNYTAX ERORR\n";
    exit(1);
}

void Parser::syntax_error()
{
    cout << "SYNTAX ERROR\n";
    exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

/**
 * Parses a regular expression and returns the REG of the regular expression that is parsed
 */
void Parser::parse_expr()
{
    Token curr_t;
    
    // No need to use while loop to check for COMMA as that is taken 
    // care of at parse_token_list
    curr_t = lexer.GetToken(); // and get another one
    // curr_t.Print();            // print token

    if (curr_t.token_type == CHAR) {
        if (curr_t.token_type == HASH) {
            // syntax_error_null();
        }
    } else {
        syntax_error_null();
    }
}

/**
 * Consume token
 */ 
void Parser::parse_token()
{
    // Notice that all token begins with an ID first other than
    // the case where totally no token is given
    Token t_peek = lexer.peek(1);

    if (t_peek.token_type != HASH) {
        Token t = expect(ID);
        parse_expr();
    } else {
        // If no token is given at all
        syntax_error_null();
    }
    
}

/**
 * Will recursively call parse_token_list until we run out of token
 */
void Parser::parse_token_list()
{
    parse_token();
    Token t = lexer.peek(1);

    if (t.token_type == COMMA) {
        expect(COMMA);
        parse_token_list();
    } else if (t.token_type == HASH) {
        return;
    } else {
        syntax_error();
    }
}

void Parser::parse_tokens_section()
{
    // Will call parse token list to store the token list
    parse_token_list();

    // HASH is used to signal end of token 
    expect(HASH);
}

/**
 * Will handle the input from the buffer
 */ 
void Parser::parse_input()
{
    // Handling Token
    parse_tokens_section();

    // After parsing all token, perform parsing on INPUT_TEXT
    Token input_token = expect(INPUT_TEXT);

    int input_text_len = input_token.lexeme.size();

    // Look at the test cases, input string is given in quotation 
    // marks and there may or may not be "" 
    string input_text_without_quote = input_token.lexeme.substr(1, input_text_len - 2);

    string formatted_input_str;

    // There may or may not be white spaces at the leading and 
    // trailing edges of input text excluding quotation marks  
    if (input_text_without_quote.at(0) == WHITE_SPACE) {
        formatted_input_str = input_text_without_quote.substr(1, input_text_without_quote.size());
    }
    if (input_text_without_quote.at(input_text_without_quote.size() - 1) == WHITE_SPACE) {
        formatted_input_str = formatted_input_str.substr(0, input_text_without_quote.size() - 1);
    }

    cout << "Input Text: " << formatted_input_str << endl;
}

// This function simply reads and prints all tokens
// I included it as an example. You should compile the provided code
// as it is and then run ./a.out < tests/test0.txt to see what this function does
void Parser::readAndPrintAllInput()
{
    Token t;

    // get a token
    t = lexer.GetToken();

    // while end of input is not reached
    while (t.token_type != END_OF_FILE)
    {
        t.Print();            // print token
        t = lexer.GetToken(); // and get another one
    }

    // note that you should use END_OF_FILE and not EOF
}

int main()
{
    // note: the parser class has a lexer object instantiated in it (see file
    // parser.h). You should not be declaring a separate lexer object.
    // You can access the lexer object in the parser functions as shown in
    // the example  method Parser::readAndPrintAllInput()
    // If you declare another lexer object, lexical analysis will
    // not work correctly
    Parser parser;
    parser.parse_input();

    // parser.readAndPrintAllInput();
}
