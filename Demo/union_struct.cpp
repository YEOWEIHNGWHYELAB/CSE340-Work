#include <iostream>

using namespace std;

union UnionTypeTest {
    int i;
    double d;
    char c;
};

struct something {
  string brand;
  string model;

  union {
    int i;
    double d;
    char c;
  };
};

struct somethingElse {
  string brand;
  string model;

  union lalala {
    int i;
    double d;
    char c;
  }; 
  
  lalala la;
};

int main() {
    UnionTypeTest u;
    u.i = 10;
    u.d = 20.2;

    something uu;
    uu.brand = "bruh";
    uu.model = "lala";
    uu.i = 10;
    uu.d = 10.2;
    uu.c = 'c';

    somethingElse uuu;
    uuu.brand = "bruh";
    uuu.model = "lala";
    uuu.la.i = 10132;
    uuu.la.d = 10.132;
    uuu.la.c = 'y';
    
    cout << "Union Type Test" << endl;
    cout << u.i << endl;
    cout << u.d << endl;

    cout << endl;

    cout << "Something Struct Test" << endl;
    cout << uu.brand << endl;
    cout << uu.model << endl;
    cout << uu.i << endl;
    cout << uu.d << endl;
    cout << uu.c << endl;

    cout << endl;

    cout << "Something Else Struct Test" << endl;
    cout << uuu.brand << endl;
    cout << uuu.model << endl;
    cout << uuu.la.i << endl;
    cout << uuu.la.d << endl;
    cout << uuu.la.c << endl;
}