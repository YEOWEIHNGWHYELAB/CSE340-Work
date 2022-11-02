#include <vector>
#include <iostream>

using namespace std;

int main() {
    vector<int> stack;
    vector<int> bruh;

    stack.push_back(2);
    stack.push_back(3);

    for (auto i = stack.rbegin(); i != stack.rend(); i++) {
        cout << *i << endl; 
    } 
    
    int currtop = *stack.end();
    cout << *stack.end() << endl;
    stack.pop_back();
    cout << *stack.end() << endl;
    stack.pop_back();
    cout << *stack.end() << endl;

    stack.push_back(2);
    stack.push_back(3);
    int n = stack.size();
    cout << stack.at(n - 1) << endl;
    bruh.push_back(stack.at(n - 1));
    int m = bruh.size();
    cout << bruh.at(m - 1) << endl;
    stack.pop_back();
    cout << bruh.at(m - 1) << endl;
}