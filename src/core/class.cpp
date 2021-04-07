#include "class.hpp"

/** Pred */
bool Pred::overlap(const Pred& other) {
  return mguPred(*this, other).has_value();
}

/* substitutions */
Pred Pred::apply_subst(const Substs& s) {
  return IsIn(t.cl, Substs::apply(s, p.ty));
}
std::vector<TyVar> Pred::tv() {
  return this->ty->tv();
}
std::vector<TyVar> Pred::tv(const std::vector<Pred>& ps);

expected<Substs, string> mguPred(const Pred& p1, const Pred& p2) {
  if (p1.cl != p2.cl)
    return {};
  return mgu(p1.ty, p2.ty);
}
expected<Substs, string> mguMatch(const Pred& p1, const Pred& p2) {
  if (p1.cl != p2.cl)
    return {};
  return match(p1.ty, p2.ty);
}

/** Qual */

Qual Qual::apply_subst(const Substs& s);
std::vector<TyVar> Qual::tv();

option<Class::super_type> ClassEnv::super(const Id& id) {
  auto it = this->classes.find(id);
  if (it == this->classes.end())
    return {};
  return it->supers;
}
option<Class::insts_type> ClassEnv::insts(const Id& id) {
  auto it = this->classes.find(id);
  if (it == this->classes.end())
    return {};
  return it->insts;
}

bool ClassEnv::defined(const Id& a) { return classes.contains(a); }
ClassEnv ClassEnv::modify(const Id& i, const Class& c) {
  auto supers = this->supers;
  auto [it, s] = supers.insert(i, c);
  if (!s) { it->first = i; it->second = c; }
  return ClassEnv(supers, this->insts);
}

expected<ClassEnv, string> ClassEnv::addClass(const Id& i, const std::vector<Id>& is) {
  if (this->defined(id)) {
    return make_unexpected(std::format("class `{}` already defined", i.to_string()));
  }
  if (std::any_of(is.cbegin(), is.cend(), [this&](const Id& i){ return !this.defined(i); })) {
    return make_unexpected(std::format("superclass not defined")); // @TODO report name
  }
  return modify(i, Class(is));
}
expected<ClassEnv, string> ClassEnv::addInst(const std::vector<Pred>& ps, const Pred& p) {
  auto i = p.cl;
  if (!this->defined(i)) {
    return make_unexpected(std::format("no class for instance (`{}`).", p.to_string()));
  }
  if (std::any_of( this->insts.cbegin()
                 , this->insts.cend()
                   , [this, p](const Pred& q){ return q.overlap(p); })) {
    return make_unexpected(std::format("overlapping instances `{}` and `@TODO`", p.to_string()));
  }
  auto insts = this->insts(i);
  insts.push_back(Qual(ps, p));
  return modify(this->super(i), insts);
}
