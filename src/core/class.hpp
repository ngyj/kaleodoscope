#include <prelude.hpp>
#include "type.hpp"
#include "subst.hpp"

/* example:
 * `(Num a) => a -> Int` is represented by
 * ```
 * Qual( [ IsIn( “Num”, Type::Var( Tyvar(“a”, Star) ) )]
 *     , Type::Fn( Type::Var( Tyvar(“a”, Star) ), tInt )
 *     )
 * ```
 */
struct Pred {
  Id cl;
  Rc<Type> ty;

// @TODO Eq
  bool overlap(const Pred& other) {
    return mguPred(*this, other).has_value();
  }
};

Pred apply_subst(const Substs& s, const Pred& p) {
  return IsIn(t.cl, Substs::apply(s, p.ty));
}
std::vector<TyVar>(const Pred& p) { return Substs::tv(p.ty); }
std::vector<TyVar> tv(const std::vector<Pred>& ps);

template<typename T>
struct Qual {
  std::vector<Pred> ctx;
  T hd;


// @TODO Eq
};

Pred IsIn(Id a, Rc<Type> ty) {
  return Pred(a, ty);
}

Qual apply_subst(const Substs& s, const Rc<Qual<T>>& q);
std::vector<TyVar> tv(const Rc<Qual<T>>& q);



optional<Substs> mguPred(const Pred& p1, const Pred& p2) {
  if (p1.cl != p2.cl)
    return {};
  return mgu(p1.ty, p2.ty);
}
optional<Substs> mguMatch(const Pred& p1, const Pred& p2) {
  if (p1.cl != p2.cl)
    return {};
  return match(p1.ty, p2.ty);
}


using Inst = Rc<Qual<Pred>>;

struct Class {
  /// super classes
  std::vector<Id> supers;
  /// instances
  std::vector<Inst> insts;
};

struct ClassEnv {
  std::map<Id, Class> classes;
  std::vector<Rc<Type>> defaults;

  std::vector<Id> super(Id);
  std::vector<Inst> insts(Id);

  bool defined(Id a) { return classes.contains(a); }
  ClassEnv modify(Id a, Class c);
  optional<ClassEnv> addClass(const Id&, const std::vector<Id>&);
  optional<ClassEnv> addInst(const std::vector<Pred>& p, const Pred& p);
}
