/**
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __MYLEXER__H__
#define __MYLEXER__H__

#include <vector>
#include <set>
#include <string>

#include "inputbuf.h"
#include "lexer.h"

using namespace std;

// Single REG Node
typedef struct RegNode {
  RegNode * first_neighbor;
  char first_label;
  RegNode * second_neighbor;
  char second_label;

  int node_number;
    
  // Each node should have at most 2 outgoing edge 
  // pointing to its neighbouring node
  RegNode() {
    first_neighbor = NULL;
    first_label = '\0';

    second_neighbor = NULL;
    second_label = '\0';
  }
} RegNode;

// Pointer to the first and last node 
// To Reference Entire Graph by start and end
typedef struct REG {
  RegNode * start;
  RegNode * accept;
} REG;

// The particular token name along with its REG
typedef struct TOKEN_REG {
  string token_name;
  int line_number;
  REG * reg;
} TOKEN_REG;

class myLexicalAnalyzer {
  public:
    // Main REGEX matching functions 
    void my_GetToken();
    int match(REG * reg, string, int);
    set<RegNode> match_one_char(set<RegNode> S, char c);

    // Split string (by delimiter) and get tokenlist
    vector<string> split(string str, char delimit);
    const vector<TOKEN_REG> &getTokensList() const;

    // Check if a node is in the given set S
    bool isNodeInSetS(set<RegNode> S, RegNode * r);

    // Check if 2 sets are equal
    bool compare_set(set<RegNode> S1, set<RegNode> S2);

    // Escape quote
    string esc_quote = "\"";

    // Input String Setter and Token List Adder
    void setCurrentInputString(const string &input_string);
    void addTokenToTokenList(const TOKEN_REG &token_reg);

    // Node's numbering
    int getCurrentNodeNumber() const;
    static void setCurrentNodeNumber(int node_number);

  private:
      vector<TOKEN_REG> tokens_list;
      string input_string;
      static int node_number;
};

#endif  // __MYLEXER__H__
