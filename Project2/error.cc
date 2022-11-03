#include <iostream>
#include "error.h"

using namespace std;

void syntax_error() {
    cout << "SNYATX EORRR !!!" << endl;
    exit(1);
}

void type_error() {
    cout << "Expression type error :(" << endl;
}

void assignment_error() {
    cout << "The following assignment(s) is/are invalid :(" << endl;
}

void valid_type() {
    cout << "Amazing! No type errors here :)" << endl;
}
