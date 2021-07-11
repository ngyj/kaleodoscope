#include <iostream>
#include <set>
#include <string>

#include "core/type.hpp"

int main() {
  std::string* ptr1 = new std::string("foo");
  std::string* ptr2 = new std::string("foo");

  std::string& ref1 = *ptr1;
  std::string& ref2 = *ptr2;
  std::string* pref1 = &ref1;

  std::string val1 = *ptr1;
  std::string val2 = *ptr2;

  std::cout << (ptr1 == ptr2) << '\n'; // false
  std::cout << (ref1 == ref2) << '\n'; // true
  std::cout << (val1 == val2) << '\n'; // true
  std::cout << "-----\n";

  std::cout << ((std::string*)&ref1 == ptr1) << '\n'; // true
}
