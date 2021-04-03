#include <iostream>
#include <prelude.hpp>
#include <tuple>
#include <variant>
#include <vector>

using var = std::variant<std::string, int, std::monostate>;
void foo(std::vector<std::string>& vs, var& v) {
  std::visit(overloaded {
      [&vs](const std::string& s) { vs.push_back(s); },
      [&vs](auto _) { vs.push_back("kek"); }
    }, v);
}

int main() {
  var s = std::string("fuck");
  var i = 1;
  var m = std::monostate{};
  auto vs = std::vector<std::string> {};
  foo(vs, i);
  foo(vs, s);
  foo(vs, m);
  std::get<0>(s)[3] = 'c';
  for (auto& i : vs) {
    std::cout << i << std::endl;
  }
  std::cout << std::get<0>(s) << std::endl;
}
//
