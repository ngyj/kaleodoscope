#include <iostream>
#include <set>
#include <string>

#include "core/name.hpp"

int main() {
  using namespace mangekyou::name;
  // FastString::init();

  auto foo  = FastString("foo");
  auto bar  = FastString("bar");
  auto foo2 = FastString("foo");
  std::cout << "foo == foo  : " << (foo == foo) << std::endl;
  std::cout << "foo == foo2 : " << (foo == foo2) << std::endl;
  std::cout << "bar == foo  : " << (bar == foo) << std::endl;

  for (auto& i : FastString::_table) {
    std::cout << i << std::endl;
  }
}
//
