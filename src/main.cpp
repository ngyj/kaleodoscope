#include <iostream>
#include <string>
#include <set>


std::set<std::string> table = std::set<std::string> {};

struct FastString {
  using table_type = std::set<std::string>;

  table_type::iterator str;

  FastString(const char* str) : str(table.emplace(str).first) {}
  FastString(const std::string& str) : str(table.emplace(str).first) {}

  std::string string() {
    return *(this->str);
  }

  bool operator==(FastString& other) { return this->str == other.str; }
  bool operator!=(FastString& other) { return this->str != other.str; }
};


int main() {
  auto foo = FastString("foo");
  auto bar = FastString("bar");
  auto foo2 = FastString("foo");
  std::cout << "foo == foo : " << (foo == foo) << std::endl;
  std::cout << "foo == foo2 : " << (foo == foo2) << std::endl;
  std::cout << "foo == foo : " << (bar == foo) << std::endl;
}
//
