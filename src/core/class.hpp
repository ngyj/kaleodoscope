#include "subst.hpp"
#include "type.hpp"
#include <expected>
#include <prelude.hpp>

namespace mangekyou::tc {

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

  Pred(Id cl, Rc<Type> ty)
      : cl(cl)
      , ty(ty) {}
  bool overlap(const Pred& other);

  /* substitutions */
  Pred apply_subst(const Substs& s);
  std::vector<TyVar> tv();
  static std::vector<TyVar> tv(const std::vector<Pred>& ps);

  // @TODO Eq
};

Pred IsIn(Id a, Rc<Type> ty) { return Pred(a, ty); }

template <typename T>
struct Qual {
  std::vector<Pred> ctx;
  T hd;

  Qual(const T& hd)
      : ctx(std::vector<Pred>{})
      , hd(hd) {}
  Qual(const std::vector<Pred>& ctx, const T& hd) ctx(ctx), hd(hd) {}

  Qual apply_subst(const Substs& s, const Rc<Qual<T>>& q);
  std::vector<TyVar> tv(const Rc<Qual<T>>& q);

  // @TODO Eq
};

tl::expected<Substs, string> mguPred(const Pred& p1, const Pred& p2);
tl::expected<Substs, string> mguMatch(const Pred& p1, const Pred& p2);

using Inst = Rc<Qual<Pred>>;

template <typename Ps, typename T>
Inst make_inst(Ps&& ctx, T&& t) {
  return make_shared<Qual<Pred>>(std::forward(ctx), std::forward(t));
}
template <typename T>
Inst make_inst(T&& t) {
  return make_shared<Qual<Pred>>(std::forward(t));
}

struct Class {
  using super_type = std::vector<Id>;
  using insts_type = std::vector<Inst>;
  /// super classes
  super_type supers;
  /// instances
  insts_type insts;

  Class(const super_type& supers)
      : supers(supers)
      , insts(std::vector{}) {}
  Class(const insts_type& insts)
      : supers(std::vector{})
      , insts(insts) {}
  Class(const super_type& supers, const insts_type& insts)
      : supers(supers)
      , insts(insts) {}
};

struct ClassEnv {
  std::map<Id, Class> classes;
  std::vector<Rc<Type>> defaults;

  ClassEnv(const std::map<Id, Class>& classes,
           const std::vector<Rc<Type>>& defaults)
      : classes(classes)
      , defaults(defaults) {}

  option<Class::super_type> super(Id);
  option<Class::insts_type> insts(Id);

  inline bool defined(Id a);
  /// creates new env with a new class
  ClassEnv modify(Id a, Class c);

  /// creates a new env with added class
  expected<ClassEnv, string> addClass(Id, const std::vector<Id>&);
  expected<ClassEnv, string> addInst(const std::vector<Pred>& p, const Pred& p);
}
} // namespace mangekyou::tc
