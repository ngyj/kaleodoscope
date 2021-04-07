#include <prelude.hpp>

#include <map>
#include <set>
#include <tuple>

#include "type.hpp"

namespace mangekyou::tc {
// using Subst  = std::tuple<TyVar, Type>;
struct Subst : std::map<TyVar, Rc<Type>> {
  using self_type = std::map<TyVar, Rc<Type>>;

  static Subst nullSubst() { return self_type(); };
  static Subst make(TyVar tv, const Rc<Type>& t) { return self_type{{tv, t}}; }

  /// `apply (compose s1 s2) = apply s1 . apply s2`
  static Subst compose(const Subst& s1, const Subst& s2);
  // parallel without left-bias (hence why it can fail)
  static expected<Subst, string> merge(const Subst& s1, const Subst& s2);
};

/// most general unifier
expected<Subst, string> mgu(Rc<Type> t1, Rc<Type> t2);

/// special case variable unification
exptected<Subst, string> varBind(Tyvar tv, Rc<Type> t);

/// find substitution s such that apply(s, t1) = t2
expected<Subst, string> match(Rc<Type> t1, Rc<Type> t2);

} // namespace mangekyou::tc
