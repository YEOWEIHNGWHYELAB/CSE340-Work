#include "iostream"
#include "execute.h"
#include "addresser.h"
#include <unordered_map>


using namespace std;

unordered_map<string, int> variable_index_map;

void next_avaiable_update() {
    next_available += 1;
}

void memory_allocation(string var_name) {
    variable_index_map.insert(pair<string, int>(var_name, next_available));
    next_avaiable_update();
}

int location(string var_name) {
    return variable_index_map.at(var_name);
}

void assign_mem(string var_name, int new_value) {
    mem[location(var_name)] = new_value;
}
