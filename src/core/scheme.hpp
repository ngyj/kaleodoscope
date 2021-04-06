#include <prelude.hpp>

#include "type.hpp"
#include "subst.hpp"
#include "class.hpp"

namespace mangekyou::tc {
/// type scheme
/// @IMPROVEMENT assumes no explicit foralls,
/// assumes `Type::Gen`'s kind is `Scheme::kinds[Type::Gen::i]`.
/// even the paper admits it sucks
struct Scheme {
  std::vector<Kind> kinds;
  Qual<Type> ty;

  Scheme(const std::vector<Kind>& kinds, const Qual<Type>& ty)
    : kinds(kinds), ty(ty) {}

  static from_scheme(const Rc<Type>& ty) { return Scheme(std::vector<TyVar>{}, Qual(ty)); }
  static Scheme quantify(std::vector<TyVar> tvs, const Qual<Type>& qt);
};

Scheme apply(const Substs& s, const Scheme& sch) {
  return Scheme(sch.kinds, apply(s, sch.ty));
}
std::vector<TyVar> tv(const Scheme& sch) { return tv(sch.ty); }


// @TODO move in Type ?
Scheme to_scheme(const Rc<Type>& ty) {
return Scheme::from_type(ty);
}

/// Assumptions pair type variables to schemes
struct Ass {
  Id id;
  Rc<Scheme> sch;
}

Ass apply(const Substs&, const Ass&);
std::vector<TyVar> tv(const Scheme&);

expected<Scheme, string> find(const Id& id, const std::vector<Ass>& asses);

} // namespace mangekyou::tc
