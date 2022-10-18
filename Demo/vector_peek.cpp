#include <iostream>
#include <vector>

using namespace std;

int main() {
    vector<int> stack;
    stack.push_back(1);
    stack.push_back(2);
    stack.push_back(3);

    cout << stack.back() << endl;
    cout << stack.back() << endl;

    auto ii = stack.end();

    ii -= 1;
    cout << *ii << endl;

    ii -= 1;
    cout << *ii << endl;
}