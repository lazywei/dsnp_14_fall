#include <iostream>
#include <string>
#include "p1a.h"

using namespace std;

void P1a::assign(const string& s) {
   _str = s;
}

void P1a::print() const {
   cout << _str << endl;
}

P1a& P1a::append(const P1a& p) {
   _str.append(p._str);
   return *this;
}
