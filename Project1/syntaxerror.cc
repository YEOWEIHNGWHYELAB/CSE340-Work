#include <iostream>
#include <cstdlib>
#include "parser.h"

using namespace std;

void Parser::syntax_error_general() {
    cout << "SNYTAX ERORR" << endl;
    exit(1);
}

void Parser::syntax_error_sem(vector<string> sem_error_str) {
    for (string curr_str : sem_error_str) {
        cout << curr_str << endl;
    }

    exit(1);
}

void Parser::syntax_error_expr(string token_error) {
    cout << "SYNTAX ERROR IN EXPRESSION OF " + token_error << endl;
    exit(1);
}

void Parser::syntax_error_epsilon(string token_error) {
    cout << "EPSILON IS NOOOOOOOT A TOKEN !!!" + token_error << endl;
    exit(1);
}

void Parser::syntax_error_input() {
    cout << "ERROR" << endl;
    exit(1);
}
