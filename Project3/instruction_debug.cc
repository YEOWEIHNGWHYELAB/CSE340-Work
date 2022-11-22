#include <iostream>
#include "execute.h"
#include "addresser.h"


using namespace std;

void print_border() {
    cout << "=============================================" << endl;
}

void print_type(struct InstructionNode * program) {
    if (program->type == NOOP) {
        cout << "NOOP TYPE" << endl;
    } else if (program->type == IN) {
        cout << "IN TYPE" << endl;
    } else if (program->type == OUT) {
        cout << "OUT TYPE" << endl;
    } else if (program->type == ASSIGN) {
        cout << "ASSIGN TYPE" << endl;
    } else if (program->type == CJMP) {
        cout << "CJMP TYPE" << endl;
    } else if (program->type == JMP) {
        cout << "JMP TYPE" << endl;
    } else {
        cout << "INVALID TYPE!" << endl;
    }
}

void print_op(ArithmeticOperatorType op) {
    if (op == OPERATOR_PLUS) {
        cout << "+";
    } else if (op == OPERATOR_MINUS) {
        cout << "-";
    } else if (op == OPERATOR_MULT) {
        cout << "*";
    } else if (op == OPERATOR_DIV) {
        cout << "/";
    } else {
        cout << "NOOP";
    }
}

void print_assign_inst(struct InstructionNode* program) {
    print_border();

    print_type(program);
    
    // Print assignment statement
    cout << program->assign_inst.left_hand_side_index;
    cout << " = ";

    if (program->assign_inst.op == OPERATOR_NONE) {
        cout << program->assign_inst.opernd1_index;
    } else {
        cout << mem[program->assign_inst.opernd1_index];
        cout << " ";
        print_op(program->assign_inst.op);
        cout << " ";
        cout << mem[program->assign_inst.opernd2_index];
    }

    cout << endl;

    print_border();
}

void print_input_inst(struct InstructionNode* program) {
    print_border();
    print_type(program);

    cout << "Variable Index: ";
    cout << program->input_inst.var_index << endl;

    cout << "Current content of mem: ";
    cout << mem[program->input_inst.var_index] << endl;

    cout << "New INPUT assignment: ";
    cout << inputs[program->input_inst.var_index] << endl; 
    
    print_border();
}

void print_output_inst(struct InstructionNode* program) {
    print_border();

    print_type(program);

    cout << "Variable Index: ";
    cout << program->output_inst.var_index << endl;

    cout << "Content of mem: ";
    cout << mem[program->output_inst.var_index] << endl;

    print_border();
}
