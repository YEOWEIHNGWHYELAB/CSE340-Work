#include <bits/stdc++.h>
using namespace std;

typedef num { END_OF_FILE, NUM, ID } TokenType;

int main() {
    TokenType t = NUM;

    if (t == END_OF_FILE) {
        cout << "EOF found";
    } else {
        cout << "Token is not EOF";
    }
}
