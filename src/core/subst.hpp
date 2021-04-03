#include <set>
#include <tuple>

#include "type.hpp"
#include <prelude.hpp>

// using Subst  = std::tuple<TyVar, Type>;
using Substs = std::map<TyVar, Type>;

Substs nullSubsts(){return std::set<Subst>::empty()};

/// should be kkind preserving
Subst make_subst(const Rc<TyVar>&, const Rc<Type>& Type);

namespace Types {
Type apply(Substs& substs, const Type& t) {
  return std::visit(overloaded{
      [substs&](TyVar& tv) {
        auto mt = substs.find(tv);
        if (mt == substs.end()) {
          return t;
        }
        return *mt;
      },
      [](TyApp& ta) { return Type::make_TyApp(apply(substs, lhs), apply(substs, rhs)); }
      [](auto&) { return t; }
    }, substs);
}

std::vector<TyVar> tv(Type& t) {
  void go(std::vector<TyVar>& vs, Type& t) {
    std::visit(overloaded {
        [vs&](const TyVar& tv) { v.push_back(tv); },
        [vs&](const TyApp& ta) { go(vs, ta.lhs); go(vs, ta.rhs); },
        [](auto&&) { }
      }, t);
  }
  auto v = std::vector();
  // std::visit([v&](auto& arg) { arg.tv(v); }, t);
  return v;
}
} // namespace Types
