#include <iostream>
#include <set>
#include <string>

#include "core/type.hpp"

int main() {
  std::cout << "hi" << "\n";
  std::cout << std::flush;

  using namespace mangekyou::name;
  using namespace mangekyou;

  std::cout << (FastString("()").string() == FastString("()").string())
            << '\n';
  std::cout << (FastString("()").string() == FastString(")(").string())
            << '\n';

  std::cout << Type::Unit->to_string() << "\n";
  std::cout << std::flush;

  std::cout << Type::Char->to_string() << "\n";
  // std::cout << tUnit.kind().to_string() << "\n";
  // std::cout << tUnit.kind() == tInt.kind() ? "true" : "false" << "\n"
}
//
