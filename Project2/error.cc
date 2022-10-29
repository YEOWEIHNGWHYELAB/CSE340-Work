#include <iostream>
#include "error.h"

using namespace std;

void syntax_error() {
    cout << "SNYATX EORRR !!!" << endl;
    exit(1);
}

void valid_type() {
    cout << "Amazing! No type errors here :)" << endl;
}

void type_error() {
    cout << "Disappointing expression type error :(" << endl;
    cout << endl;
}
