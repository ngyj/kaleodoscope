#include "subst.hpp"

namespace mangekyou::tc {
Subst Subst::compose(const Subst& s1, const Subst& s2) {
  auto substs = s2;
  for (auto& [v, t] : substs) {
    t = apply(s1, t);
  }
  for (auto kv : s1) {
    // @QUESTION what to do in case of conflicting keys?
    s2.insert(kv);
  }
}

expected<Subst, string> Subst::merge(const Subst& s1, const Subst& s2) {
  auto set = std::set<TyVar>();
  for (const auto [k, _] : s1) {
    set.insert(k);
  }
  for (const auto [k, _] : s2) {
    set.insert(k);
  }
  for (auto i : set) {
    auto tv = Types::TyVar(i);
    if (apply(s1, tv) != apply(s2, tv))
      return make_unexpected("merge failed");
  }
  auto s = s1;
  for (auto kv : s2) {
    s2.insert(s1);
  }
  return expected(s2);
}

expected<Subst, string> mgu(Rc<Type> t1, Rc<Type> t2) {
  return make_unexpected("@TODO");
}

exptected<Subst, string> varBind(Tyvar tv, Rc<Type> t) {
  return make_unexpected("@TODO");
}

expected<Subst, string> match(Rc<Type> t1, Rc<Type> t2) {
  if (t1.is<TyApp>() && t2.is<TyApp>()) {
    auto sl = match(t1.get<TyApp>().lhs, t2.get<TyApp>().lhs);
    if (!sl)
      return sl;
    auto sr = match(t1.get<TyApp>().rhs, t2.get<TyApp>().rhs);
    if (!sr)
      return sr;
    return Subst::merge(*sl, *sr);

  } else if (t1.is<TyCon>() && t2.is<TyCon>() && t1 == t2) {
    return Subst::nullSubst();

  } else if (t1.is<TyVar>() && t1.kind() == t2.kind()) {
    return Subst::make_subst(t1.get<TyVar>(), t2);

  } else {
    return "could not match types";
  }
}

} // namespace mangekyou::tc
