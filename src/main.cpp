#include <iostream>
#include <prelude.hpp>
#include <map>

int main() {
   auto xs = std::map<int, std::string>{{0, "foo"}, {3, "bar"}};
   auto ys = xs;
   for (auto& [k, v] : ys) {
     v = "fuck";
   }
   ys.insert({2, "baz"});
   xs.insert({10, "qux"});
   for (const auto& kv : xs) {
     ys.insert(kv);
   }
   std::cout << "xs:" << std::endl;
   for (auto [k, v] : xs) {
     std::cout << "  " << k << ", " << v << std::endl;
   }
   std::cout << "ys:" << std::endl;
   for (auto [k, v] : ys) {
     std::cout << "  " << k << ", " << v << std::endl;
   }
}
//
