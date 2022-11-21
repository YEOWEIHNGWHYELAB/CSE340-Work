#include <unordered_map>


using namespace std;

extern unordered_map<string, int> variable_index_map;

void memory_allocation(string var_name);
int location(string var_name);
void assign_mem(string var_name);
