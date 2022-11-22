#include <iostream>

#include "addresser.h"
#include "execute.h"
#include "lexer.h"


using namespace std;

// lexer for reading buffer input
LexicalAnalyzer lexer;

struct InstructionNode* parse_if_stmt();

int parse_primary() {
    Token curr_token = lexer.GetToken();

    if (curr_token.token_type == ID) {
        // ID
        return location(curr_token.lexeme);
    } else {
        // NUM
        memory_constant_allocation(curr_token.lexeme);
        return location(curr_token.lexeme);
    }
}

struct InstructionNode* parse_assign_stmt(Token id_token) {
    // Create the new assignment instrcution node
    struct InstructionNode* assignment_instnode = new InstructionNode;
    assignment_instnode->type = ASSIGN;
    assignment_instnode->next = nullptr;

    // What variable to assign to?
    assignment_instnode->assign_inst.left_hand_side_index = location(id_token.lexeme);

    lexer.expect(EQUAL);

    /**
     * Can be primary or expr
     * 
     * expr -> primary op primary
    */
    int first_primary_index = parse_primary();

    // Could be op or SEMICOLON
    Token next_token = lexer.GetToken();
    
    /**
     * Check if it is:
     * 
     * assign stmt -> ID EQUAL primary SEMICOLON
     * assign stmt -> ID EQUAL expr SEMICOLON
    */
    if (next_token.token_type == SEMICOLON) {
        // next_token is a SEMICOLON

        assignment_instnode->assign_inst.op = OPERATOR_NONE;
        assignment_instnode->assign_inst.opernd1_index = first_primary_index;
    } else {
        // next_token is a op

        int second_primary_index = parse_primary();

        if (next_token.token_type == PLUS) {
            assignment_instnode->assign_inst.op = OPERATOR_PLUS;
        } else if(next_token.token_type == MINUS) {
            assignment_instnode->assign_inst.op = OPERATOR_MINUS;
        } else if(next_token.token_type == MULT) {
            assignment_instnode->assign_inst.op = OPERATOR_MULT;
        } else {
            // DIV
            assignment_instnode->assign_inst.op = OPERATOR_DIV;
        }

        assignment_instnode->assign_inst.opernd1_index = first_primary_index;
        assignment_instnode->assign_inst.opernd2_index = second_primary_index;

        lexer.expect(SEMICOLON);
    }

    return assignment_instnode;
}

struct InstructionNode* parse_input_stmt() {
    /**
     * input stmt -> input ID SEMICOLON
    */

    // ID
    Token input_token = lexer.GetToken();

    struct InstructionNode* input_instnode = new InstructionNode;
    input_instnode->type = IN;
    input_instnode->input_inst.var_index = location(input_token.lexeme);
    input_instnode->next = nullptr;

    /*
    mem[location(input_token.lexeme)] = inputs[next_input];
    next_input = next_input + 1;
    */

    lexer.expect(SEMICOLON);

    return input_instnode;
}

struct InstructionNode* parse_output_stmt() {
    /**
     * output stmt -> output ID SEMICOLON
    */

    // ID
    Token output_token = lexer.GetToken();

    struct InstructionNode* output_instnode = new InstructionNode;
    output_instnode->type = OUT;
    output_instnode->output_inst.var_index = location(output_token.lexeme);
    output_instnode->next = nullptr;

    lexer.expect(SEMICOLON);
    
    return output_instnode;
}

struct InstructionNode* parse_stmt() {
    /**
     * stmt -> assign_stmt | while_stmt | if_stmt | switch_stmt | for_stmt | output_stmt | input_stmt
    */
    Token curr_token = lexer.GetToken();

    if (curr_token.token_type == IF) {
        return parse_if_stmt();
    } else if (curr_token.token_type == ID) {
        // Assign statment starts with ID
        return parse_assign_stmt(curr_token);
    } else if (curr_token.token_type == WHILE) {
        //return parse_while_stmt();
    } else if (curr_token.token_type == SWITCH) {
        //return parse_switch_stmt();
    } else if (curr_token.token_type == FOR) {
        //return parse_for_stmt();
    } else if (curr_token.token_type == INPUT) {
        return parse_input_stmt();
    } else if (curr_token.token_type == OUTPUT) {
        return parse_output_stmt();
    }

    return nullptr;
}

struct InstructionNode* parse_stmt_list() {
    /**
     * stmt_list -> stmt stmt_list | stmt
    */

    // Construct the current instruction for the current statement
    struct InstructionNode* curr_stmt = parse_stmt();

    // See if next token is RBRACE
    Token next_token = lexer.peek(1);
    
    // instruction list for statement list
    struct InstructionNode* instList; 

    // Recursively parse statement list
    if (next_token.token_type != RBRACE) {
        instList = parse_stmt_list();
        
        // The next subsequent instruction node will be coming 
        // from next statement
        if (curr_stmt->next != nullptr) {
            struct InstructionNode* temp_instnode = curr_stmt;

            while (temp_instnode->next != nullptr) {
                temp_instnode = temp_instnode->next;
            }

            temp_instnode->next = instList;
        } else {
            curr_stmt->next = instList;
        }
    }

    return curr_stmt;
}

struct InstructionNode* parse_body() {
    /**
     * body -> LBRACE stmt list RBRACE
    */
    lexer.expect(LBRACE);
    InstructionNode* instruction_entry = parse_stmt_list();
    lexer.expect(RBRACE);

    return instruction_entry;
}

struct InstructionNode* parse_if_stmt() {
    /**
     * if_stmt -> IF condition body
    */
    struct InstructionNode * if_instnode = new InstructionNode;

    if_instnode->type = CJMP;

    if_instnode->cjmp_inst.opernd1_index = parse_primary();

    Token condtion_token = lexer.GetToken();
    if (condtion_token.token_type == GREATER) {
        if_instnode->cjmp_inst.condition_op = CONDITION_GREATER;
    } else if (condtion_token.token_type == LESS) {
        if_instnode->cjmp_inst.condition_op = CONDITION_LESS;
    } else {
        if_instnode->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
    }

    if_instnode->cjmp_inst.opernd2_index = parse_primary();

    /**
     * True branch
     * 
     * parse_body returns a pointer to a sequence of instructions
    */
    if_instnode->next = parse_body();

    /**
     * This is a node that does not result in any action being taken 
     * Make sure to set the next field to nullptr
    */
    struct InstructionNode* no_op_instnode = new InstructionNode;
    no_op_instnode->type = NOOP;
    no_op_instnode->next = nullptr;

    /**
     * Append no-op node to the body of the if
     * 
     * This requires a loop to get to the end of 
     * true_branch by following the next field 
     * you know you reached the end when next is nullptr
     * it is very important that you always appropriately
     * initialize fields of any data structures
     * do not use uninitialized pointers
    */
    while (if_instnode->next->next != nullptr) {
        if_instnode->next = if_instnode->next->next;
    }

    if_instnode->next->next = no_op_instnode;

    /**
     * False branch
     * 
     * Set inst->cjmp_inst.target to point to no-op node
    */
    if_instnode->cjmp_inst.target = no_op_instnode;

    return if_instnode;
}

void parse_num() {
    Token curr_num = lexer.GetToken();
    inputs.push_back(stoi(curr_num.lexeme));
}

void parse_num_list() {
    /**
     * num list -> NUM num_list | NUM
    */
    parse_num();

    if (lexer.peek(1).token_type == NUM) {
        parse_num_list();
    }
}

void parse_input() {
    /**
     * inputs -> num_list
    */
    parse_num_list();
}

void parse_id_list() {
    /**
     * id_list -> ID COMMA id_list | ID
    */
    Token curr_token = lexer.GetToken();

    // Allocate memory for this id token (variable)
    memory_allocation(curr_token.lexeme);

    Token next_token = lexer.peek(1);

    if (next_token.token_type == COMMA) {
        lexer.expect(COMMA);
        parse_id_list();
    }
}

void parse_var_section(){
    /**
     * var section -> id_list SEMICOLON
    */
    parse_id_list();
    lexer.expect(SEMICOLON);
}

struct InstructionNode* parse_program() {
    /**
     * program -> var_section body inputs
    */
    parse_var_section();
    InstructionNode * instruction_entry = parse_body();
    parse_input();

    return instruction_entry;
}

struct InstructionNode* parse_generate_intermediate_representation() {
    return parse_program();
}
