#include <expected>
#include <prelude.hpp>

#include <initializer_list>
#include <map>
#include <set>
#include <tuple>

#include "type.hpp"

using tl::expected;

namespace mangekyou::tc {
// using Subst  = std::tuple<TyVar, Type>;
struct Subst : std::map<TyVar, Rc<Type>> {
  using self_type = std::map<TyVar, Rc<Type>>::map;

  Subst()
      : self_type() {}
  Subst(std::initializer_list<self_type::value_type> init)
      : self_type(init) {}

  static Subst nullSubst() { return Subst{}; };
  static Subst make(TyVar tv, const Rc<Type>& t) { return Subst{{tv, t}}; }

  /// `apply (compose s1 s2) = apply s1 . apply s2`
  static Subst compose(const Subst& s1, const Subst& s2);
  // parallel without left-bias (hence why it can fail)
  static expected<Subst, string> merge(const Subst& s1, const Subst& s2);
};

/// most general unifier
expected<Subst, string> mgu(Rc<Type> t1, Rc<Type> t2);

/// special case variable unification
expected<Subst, string> varBind(TyVar tv, Rc<Type> t);

/// find substitution s such that apply(s, t1) = t2
expected<Subst, string> match(Rc<Type> t1, Rc<Type> t2);

} // namespace mangekyou::tc
