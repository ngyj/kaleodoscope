#include "../core/name.hpp"
#include "var.hpp"
#include <prelude.hpp>
#include <variant>
#include <vector>

// using TyVar = Var;
using KindOrType = Type;
using Id         = std::string;

struct Kind;

struct TyVar {
  Id id;
  Kind kind;

  TyVar(Id id, Kind kind)
      : id(id)
      , kind(kind) {}
  bool operator==(const TyVar& other) { return this->id == other.id; }
  bool operator!=(const TyVar& other) { return this->id != other.id; }
  bool operator<(const TyVar& other) { return this->id < other.id; }
};

/* Kinds */
struct Star {
  bool operator==(const Star& _) noexcept { return true; }
  bool operator!=(const Star& _) noexcept { return false; }
};
struct KArr {
  Ref<Kind> lhs;
  Ref<Kind> rhs;
  bool operator==(const KArr& other) {
    return *(this->lhs) == *(other.lhs) && *(this->rhs) == *(other.rhs);
  }
  bool operator!=(const KArr& other) {
    return *(this->lhs) != *(other.lhs) || *(this->rhs) != *(other.rhs);
  }
};

struct Kind : std::variant<Star, KArr> {
  using std::variant<Star, KArr>::variant;
}

/* Types */
struct TyCon {
  Id id;
  Kind kind;
};

// @FIXME
struct TyApp {
  Ref<Type> fun;
  Ref<Type> arg;
};
/// generic/quantified type variables
struct TGen {
  i32 i;
};

struct Type : std::variant<TyVar, TyCon, TyApp, TyGen> {
  using std::variant<TyVar, TyCon, TyApp, TyGen>::variant;
}
