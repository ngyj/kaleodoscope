#include <prelude>
#include <expected>
#include <variant>

namespace mangekyou::pat {
  /// @TODO FieldPuns and Shit
  struct Literal;
  struct Pat;

  /// binding
  struct PVar {
    Id id;
  };
  /// `_`
  struct PWildcard {};
  /// as-pattern `id@pat`
  struct PAs {
    Id id;
    Rc<Pat> pat;
  };
  /// literal pattern
  struct PLit {
    Literal lit;
  };
  /// Constructor
  struct PCon {
    Ass ass;
    std::vector<Pat> args;
  };


}
