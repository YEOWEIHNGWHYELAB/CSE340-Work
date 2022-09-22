#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stdio.h>
#include <sstream>
#include <algorithm>

#include "mylexer.h"
#include "inputbuf.h"
#include "parser.h"

using namespace std;

// Give the context of myLexicalAnalyzer's node_number
int myLexicalAnalyzer::node_number;

// Increment the node number and return the incremented node number
int myLexicalAnalyzer::getCurrentNodeNumber() const {
    return ++node_number;
}

// Set the current node number to the value specified
void myLexicalAnalyzer::setCurrentNodeNumber(int count) {
    node_number = count;
}

// Set the INPUT_TEXT given from the standard IO
void myLexicalAnalyzer::setCurrentInputString(const string &input_text) {
    myLexicalAnalyzer::input_text = input_text;
}

// Add token to the current token list
void myLexicalAnalyzer::addTokenToTokenList(const TOKEN_REG &token) {
    myLexicalAnalyzer::tokens_list.push_back(token);
}

// Returns a pointer to the current token list
const vector<TOKEN_REG> &myLexicalAnalyzer::getTokensList() const {
    return tokens_list;
}

// Split string by delimiter
vector<string> myLexicalAnalyzer::splitDelim(string str, char delimiter) {
    vector<string> string_list;
    string curr_str = "";
    stringstream strstream(str); // Turn the string into a stream.

    while (getline(strstream, curr_str, delimiter)) {
        string_list.push_back(curr_str);
    }

    return string_list;
}

// Check if a particular REG Node is in the given set, 
// this is done through the node numbering comparison
bool myLexicalAnalyzer::isNodeInSetS(set<RegNode> curr_set, RegNode * reg_node){
    for (auto set_i = curr_set.begin(); set_i != curr_set.end(); set_i++) {
        if (reg_node->node_number == set_i->node_number) {
            return true;
        }
    }

    return false;
}

// Go thorugh the set and insert each element of each set 
// into 2 separate array and do comparison to check if each
// element is equal when ordered / sorted  
bool myLexicalAnalyzer::compareSet(set<RegNode> set_1, set<RegNode> set_2) {
    // Trivial
    if (set_1.size() != set_2.size()) {
        return false;
    }

    const int set_1_size = set_1.size();
    const int set_2_size = set_2.size();

    int s1[set_1_size] = {};
    int s2[set_2_size] = {};

    int j = 0;

    for (auto set_it = set_1.begin(); set_it != set_1.end(); set_it++) {
        s1[j] = set_it->node_number;
        j += 1;
    }

    int k = 0;

    for (auto set_it = set_2.begin(); set_it != set_2.end(); set_it++) {
        s2[k] = set_it->node_number;
        k += 1;
    }

    // No need to sort the 2 list since set data 
    // structure in C++ is ordered

    for (int ii = 0; ii < set_1_size; ii++) {
        if (s1[ii] != s2[ii]) {
            return false;
        }
    }

    return true;
}

// Operator overload for RegNode Comparision
static bool operator<(const RegNode& node_1, const RegNode& node_2) {
    return node_1.node_number < node_2.node_number;
}

/**
 * Returns the set of reachable nodes after consuming consuming c
 * 
 * current_reachable_node_set corresponds to set S
 */
set<RegNode> myLexicalAnalyzer::match_one_char(set<RegNode> current_reachable_node_set, char c) {
    set<RegNode> nodes_reachable_consuming_c; // Set S'

    /**
     * From project description: 
     * 
     * for every node in S if there is an edge labeled c from n 
     * to a node m && m is not in S'
     * 
     * Then add node m to S'
     */
    for (auto set_it = current_reachable_node_set.begin(); set_it != current_reachable_node_set.end(); set_it++) {
        // Check for first neighbour 
        if (set_it->first_label == c && !isNodeInSetS(nodes_reachable_consuming_c, set_it->first_neighbor)){
            nodes_reachable_consuming_c.insert(*set_it->first_neighbor);
        }

        // Check for second neighbour 
        if (set_it->second_label == c && !isNodeInSetS(nodes_reachable_consuming_c, set_it->second_neighbor)){
            nodes_reachable_consuming_c.insert(*set_it->second_neighbor);
        }
    }

    // If set is empty even after consuming c, then return empty set
    if (nodes_reachable_consuming_c.empty()) {
        return nodes_reachable_consuming_c;
    }

    /**
     * From project description: 
     * 
     * At this point, S' is not empty and it contains the nodes that
     * can be reached from S by consuming the character c directly
     */
    bool changed = true;
    set<RegNode> node_reachable; // Nodes reachable from S' consuming no characters

    while (changed) {
        changed = false;

        // Iterating through set S' node
        for (auto set_it = nodes_reachable_consuming_c.begin(); set_it != nodes_reachable_consuming_c.end(); set_it++) {
            // add all nodes in set S' to set S''
            // You MUST include this, although set will discard duplicates,
            // your end if statement relies on this to check if the 2 sets 
            // are equal or not
            node_reachable.insert(*set_it);

            // First neighbour
            if (set_it->first_label == '_' && !isNodeInSetS(node_reachable, set_it->first_neighbor)) {
                node_reachable.insert(*set_it->first_neighbor);
            }

            // Second neighbour
            if (set_it->second_label == '_' && !isNodeInSetS(node_reachable, set_it->second_neighbor)) {
                node_reachable.insert(*set_it->second_neighbor);
            }
        }

        // Check if the set has changed (more nodes reachable while consuming '_')
        if (!compareSet(nodes_reachable_consuming_c, node_reachable)) {
            changed = true;
            nodes_reachable_consuming_c.insert(node_reachable.begin(), node_reachable.end()); // Duplicate nodes are discarded
            node_reachable.clear();
        }
    }

    /**
     * From project description: 
     * 
     * At this point the set S' contains all nodes that can be reached
     * from S by first consuming c, then traversing 0 or more epsilon
     * edges
     */
    return nodes_reachable_consuming_c;
}

/**
 * match(r,s,p) is called for every REG r in the list starting from the 
 * current position p and it returns the longest prefix match for that 
 * INPUT_TEXT part with the given REG
 */
int myLexicalAnalyzer::match(REG * reg, string str, int pos) {
    int current_input_match_len = 0;
    int longest_possible_lexeme = 0;

    set<RegNode> current_reachable_node_set;
    
    // Insert start node from REG
    current_reachable_node_set.insert(*reg->start);

    // Insert all nodes that can be reached while consuming no input
    set<RegNode> reachable_node_from_start = match_one_char(current_reachable_node_set, '_');
    current_reachable_node_set.insert(reachable_node_from_start.begin(), reachable_node_from_start.end());

    // End of input string 
    string::iterator end_of_input_str = str.end();

    for (auto input_string_iterator = str.begin() + pos; input_string_iterator != end_of_input_str; input_string_iterator++) {
        // Match one character
        current_reachable_node_set = match_one_char(current_reachable_node_set, *input_string_iterator);
        current_input_match_len += 1;
        
        // Find if you can reach the accept node, then you can match successfully!
        // And because set find will return the end of node if the accept node 
        // is not found so we check if it is at the end node of the set. 
        if (current_reachable_node_set.find(*reg->accept) != current_reachable_node_set.end()) {
            longest_possible_lexeme = current_input_match_len;
        }
    }

    return longest_possible_lexeme;
}

/**
 * Prints the token with the longest matching prefix match together with its 
 * lexeme and updates the value of the current position p
 */
void myLexicalAnalyzer::my_GetToken(){
    // Split the string into vector of strings based on the white space delimiter
    vector<string> input_string_list = splitDelim(input_text, WHITE_SPACE);
    string current_token_name = "";

    // Go through the list of input string separated by " "
    for (int current_string_index = 0; current_string_index < (int)input_string_list.size(); current_string_index++) {
        int string_start_index = 0; // The 'p' according to project description
        int longest_prefix_string_len = 0;

        // Iterating through the current string input in intervals of the current longest prefix string length
        for (string_start_index = 0; string_start_index != (int)input_string_list[current_string_index].size(); string_start_index += longest_prefix_string_len) {
            vector<TOKEN_REG> token_list = getTokensList();
            longest_prefix_string_len = 0;

            // Going through the list of token store and using that do REGEX matching with INPUT_TEXT 
            for (auto current_token_reg = token_list.begin(); current_token_reg != token_list.end(); current_token_reg++) {
                // match(r, s, p)
                int current_prefix_length = match(current_token_reg->reg, input_string_list[current_string_index], string_start_index);

                if (current_prefix_length > longest_prefix_string_len) {
                    longest_prefix_string_len = current_prefix_length;
                    current_token_name = current_token_reg->token_name;
                }
            }

            if (longest_prefix_string_len == 0) {
                Parser::syntax_error_input();
            }

            cout << current_token_name << " , " + esc_quote + input_string_list[current_string_index].substr(string_start_index, longest_prefix_string_len) + esc_quote << endl;
        }
    }
}
