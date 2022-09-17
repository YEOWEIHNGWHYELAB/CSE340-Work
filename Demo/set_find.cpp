// set::begin/end

#include <iostream>
#include <set>

int main () {
    int myints[] = {75, 23, 65, 42, 13, 11, 10};
    std::set<int> myset (myints, myints + 7);

    std::cout << "myset contains:";
    for (std::set<int>::iterator it=myset.begin(); it!=myset.end(); ++it)
        std::cout << ' ' << *it;

    std::cout << '\n';

    std::set<int>::iterator it = myset.find(69);
    std::cout << *it << std::endl;

    std::set<int>::iterator it1 = myset.begin();
    std::cout << *it1 << std::endl;

    std::set<int>::iterator it2 = myset.end();
    std::cout << *it2 << std::endl;

    return 0;
}
