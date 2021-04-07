#include "name.hpp"
#include <variant>

struct Kind;
struct Type;

struct Var {
  enum class Sort { Ty, Id };
  Name name;
  std::variant<Kind, Type> KindOrType;
};
