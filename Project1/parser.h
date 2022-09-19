/**
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>

#include "lexer.h"
#include "mylexer.h"

class Parser {
  public:
    bool has_sem_error = false;
    bool has_epsilon_error = false;
    string curr_episilon_error = "";
    string current_token_name = "";

    void parse_input();
    void readAndPrintAllInput();
    void syntax_error_general();
    void static syntax_error_input();
    void syntax_error_expr(string);
    void syntax_error_epsilon(string);
    void syntax_error_sem(vector<string>);
    
  private:
    Token expect(TokenType expected_type);
    Token expect_expr(string token_str, TokenType expected_type);

    struct REG * parse_expr();
    void parse_token();
    void parse_token_list();
    void parse_tokens_section();
};

#endif
