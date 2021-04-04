#include <set>
#include <tuple>

#include "type.hpp"
#include <prelude.hpp>

// using Subst  = std::tuple<TyVar, Type>;
struct Substs : std::map<TyVar, Rc<Type>> {
  using std::map<TyVar, Rc<Type>>::map;
  using self_type = std::map<TyVar, Rc<Type>>;

  Substs nullSubsts(){ return self_type() };

  /// `apply (compose s1 s2) = apply s1 . apply s2`
  Substs compose(const Substs& s1, const Substs& s2) {
    auto substs = s2;
    for (auto& [v, t] : substs){
      t = apply(s1, t);
    }
    for (auto kv : s1) {
      // @QUESTION what to do in case of conflicting keys?
      s2.insert(kv);
    }
  }
  optional<Susbts> merge(const Substs& s1, const Substs& s2) {
    auto set = std::set<TyVar>();
    for (const auto [k, _] : s1) { set.insert(k); }
    for (const auto [k, _] : s2) { set.insert(k); }
    for (auto i : set) {
      auto tv = Types::TyVar(i);
      if (apply(s1, tv) != apply(s2, tv))
        return {};
    }
    auto s = s1;
    for(auto kv: s2) {
      return {s2.insert(s1)};
    }
  }

};

Subst make_substs(TyVar tv, const Rc<Type>& t) {
return std::map<TyVar, Rc<Type>>{ {tv, t} };
}

/// apply substitutions
Rc<Type> apply_subst(Substs& substs, const Rc<Type>& t) {
  return std::visit(overloaded{
      [substs&, t](TyVar& tv) {
        auto mt = substs.find(tv);
        if (mt == substs.end()) {
          return t;
        }
        return *mt;
      },
      [](TyApp& ta) { return Type::make_TyApp(apply(substs, lhs), apply(substs, rhs)); }
      [t](auto) { return t; }
    }, substs);
}

/// collect typevariables (ordered) from a Type
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

// @TODO error reporting instead of just optional
/// most general unifier
optional<Substs> mgu(Rc<Type> t1, Rc<Type> t2);

// @TODO error reporting instead of just optional
/// special case variable unification
optional<Substs> varBind(Tyvar tv, Rc<Type> t);

// @TODO error reporting instead of just optional
/// find substitution s such that apply(s, t1) = t2
optional<Substs> match(Rc<Type> t1, Rc<Type> t2) {
  if (t1.is<TyApp>() && t2.is<TyApp>()) {
    auto sl = match(t1.get<TyApp>().lhs, t2.get<TyApp>().lhs);
    if (!sl)
      return sl;
    auto sr = match(t1.get<TyApp>().rhs, t2.get<TyApp>().rhs);
    if (!sr)
      return sr;
    return Substs::merge(*sl, *sr);

  } else if (t1.is<TyCon>() && t2.is<TyCon>() && t1 == t2) {
    return Substs::nullSubsts();

  } else if (t1.is<TyVar>() && t1.kind() == t2.kind()) {
    return { Substs::make_subst(t1.get<TyVar>(), t2) }

  } else {
    return {};
  }
}
