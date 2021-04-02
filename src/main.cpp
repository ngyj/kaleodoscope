#include <iostream>
#include <vector>
#include <variant>
#include <prelude.hpp>

struct Dog {
  std::string sound() {
    return "bark";
  }
};
struct Cat {
  std::string sound() {
    return "moew";
  }
};

struct Animal : std::variant<Dog, Cat> {
  using std::variant<Dog,Cat>::variant;

  std::string sound() {
    return std::visit(overloaded {
      [](Dog& arg) { return arg.sound(); },
      [](Cat& arg) { return arg.sound(); },
    }, *this);
  }
};

 
int main() {
  auto as = std::vector<Animal> {Dog{}, Dog{}, Cat{} };
  for (auto& a : as) {
    std::cout << a.sound() << std::endl;
  }
  return 0;
}
// 