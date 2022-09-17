#include <iostream>
#include <cstdlib>
#include "parser.h"

using namespace std;

void Parser::syntax_error_general() {
    cout << "SNYTAX ERORR" << endl;
    exit(1);
}

void Parser::syntax_error_sem(string line_number_new, string token_name, string line_number_orginal) {
    cout << "Line " + line_number_new + ": " + token_name + " already declared on line " + line_number_orginal << endl;
}

void Parser::syntax_error_expr(string token_error) {
    cout << "SYNTAX ERROR IN EXPRESSION OF " + token_error << endl;
    exit(1);
}

void Parser::syntax_error_epsilon(string token_error) {
    cout << "EPSILON IS NOOOOOOOT A TOKEN !!! " + token_error << endl;
    exit(1);
}
