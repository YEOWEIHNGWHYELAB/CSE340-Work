#include <iostream>
#include <map>

using namespace std;

typedef enum {
    PREC_LESS, 
    PREC_GREATER, 
    PREC_EQUAL, 
    PREC_ERR, 
    PREC_ACCEPT
} precedenceValue;

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

typedef enum { 
  END_OF_FILE = 0,
  SCALAR, 
  ARRAY, 
  OUTPUT, 
  PLUS, 
  MINUS, 
  DIV, 
  MULT,
  EQUAL, 
  SEMICOLON,
  LBRAC, 
  RBRAC, 
  LPAREN, 
  RPAREN, 
  LBRACE, 
  RBRACE,
  DOT, 
  NUM, 
  ID, 
  ERROR
} TokenType;

class Token {
  public:
    void Print();

    std::string lexeme;
    TokenType token_type;
    int line_no;
};

int main() {
    map<int, int> mp_index_table;
    mp_index_table.insert(pair<int, int>(PLUS, 0));
    mp_index_table.insert(pair<int, int>(MINUS, 1));
    mp_index_table.insert(pair<int, int>(MULT, 2));
    mp_index_table.insert(pair<int, int>(DIV, 3));
    mp_index_table.insert(pair<int, int>(LPAREN, 4));
    mp_index_table.insert(pair<int, int>(RPAREN, 5));
    mp_index_table.insert(pair<int, int>(LBRAC, 6));
    mp_index_table.insert(pair<int, int>(DOT, 7));
    mp_index_table.insert(pair<int, int>(RBRAC, 8));
    mp_index_table.insert(pair<int, int>(NUM, 9));
    mp_index_table.insert(pair<int, int>(ID, 10));
    mp_index_table.insert(pair<int, int>(END_OF_FILE, 11));

    Token* t1 = new Token();
    t1->lexeme = "a";
    t1->token_type = ID;
    t1->line_no = 1;

    Token* t2 = new Token();
    t1->lexeme = ".";
    t1->token_type = DOT;
    t1->line_no = 1;

    cout << mp_index_table[t1->token_type] << endl;
    cout << mp_index_table[t2->token_type] << endl;

    if (precedence_table[mp_index_table[t1->token_type]][mp_index_table[t2->token_type]] == PREC_ERR) {
        cout << "CORRECT ERROR RESPONSE" << endl;
    } else {
        cout << "FAILED" << endl;
    }
}
