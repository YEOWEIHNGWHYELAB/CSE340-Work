#include <iostream>
#include <unordered_map>

#include "execute.h"
#include "addresser.h"


using namespace std;

unordered_map<string, int> variable_index_map;

void next_avaiable_update() {
    next_available += 1;
}

void assign_mem(string var_name, int new_value) {
    mem[location(var_name)] = new_value;
}

void memory_allocation(string var_name) {
    variable_index_map.insert(pair<string, int>(var_name, next_available));
    next_avaiable_update();
}

void memory_constant_allocation(string constant) {
    // If constant has already been declared before
    if (variable_index_map.find(constant) == variable_index_map.end()) {
        variable_index_map.insert(pair<string, int>(constant, next_available));
        assign_mem(constant, stoi(constant));
        next_avaiable_update();
    }
}

int location(string var_name) {
    return variable_index_map.at(var_name);
}
