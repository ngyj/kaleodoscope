#include "../core/name.hpp"
#include "var.hpp"
#include <prelude.hpp>
#include <variant>
#include <vector>

namespace mangekyou {

// using TyVar = Var;
using KindOrType = Type;
using Id         = std::string;

struct Kind;

/* Kinds */
struct Star {
  bool operator==(const Star& _) noexcept { return true; }
  bool operator!=(const Star& _) noexcept { return false; }
};
struct KArr {
  Rc<Kind> lhs;
  Rc<Kind> rhs;

  KArr(const Rc<Kind>& lhs, const Rc<Kind>& rhs) : lhs(lhs), rhs(rhs) {}

  bool operator==(const KArr& other) {
    return *(this->lhs) == *(other.lhs) && *(this->rhs) == *(other.rhs);
  }
  bool operator!=(const KArr& other) {
    return *(this->lhs) != *(other.lhs) || *(this->rhs) != *(other.rhs);
  }
};

struct Kind : std::variant<Star, KArr> {
  using std::variant<Star, KArr>::variant;

  Arrow(const Rc<Kind>& lhs, const Rc<Kind>& rhs);

  static const Kind STAR = Star{};
}

/* Types */
struct TyVar {
  Id id;
  Kind _kind;

  TyVar(Id id, Kind _kind) : id(id) , _kind(_kind) {}

  Kind kind();

  bool operator==(const TyVar& other) { return this->id == other.id; }
  bool operator!=(const TyVar& other) { return this->id != other.id; }
  bool operator<(const TyVar& other) { return this->id < other.id; }
};

struct TyCon {
  Id id;
  Kind _kind;

  TyCon(Id id, Kind _kind) : id(id), _kind(kind) {}

  Kind kind();
};

// @FIXME
struct TyApp {
  Ref<Type> lhs;
  Ref<Type> rhs;

  TyApp(const Rc<Type>& lhs, const Rc<Type>& rhs) lhs(lhs), rhs(rhs) {}

  Kind kind();
};
/// generic/quantified type variables
struct TyGen {
  i32 i;

  TyGen(i32 i) : i(i) {}

  Kind kind();
};

struct Type : std::variant<TyVar, TyCon, TyApp, TyGen> {
  using std::variant<TyVar, TyCon, TyApp, TyGen>::variant;

  Type Type::Var(Id i, Kind k);
  Type Type::Con(Id i, Kind k);
  Type Type::App(const Rc<Type>& lhs, Rc<Type>& rhs);
  Type Type::Gen(i32 i);

  Kind kind() {
    return std::visit([](auto&& arg) { return arg.kind(); }, *this);
  }
}
}

/// type scheme
/// @IMPROVEMENT assumes no explicit foralls,
/// assumes `Type::Gen`'s kind is `Scheme::kinds[Type::Gen::i]`.
/// even the paper admits it sucks
struct Scheme {
  std::vector<Kind> kinds;
  // @TODO
  Qual<Type> ty;
}
