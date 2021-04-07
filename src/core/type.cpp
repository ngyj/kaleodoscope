#include "type.hpp"
#include "subst.hpp"
#include <variant>

namespace mangekyou {
bool KArr::is_equal(const KArr& rhs) const {
  return *(this->lhs) == *(rhs.lhs) && *(this->rhs) == *(rhs.rhs);
}
std::string KStar::to_string() const { return "Kind::Star"; }
std::string KArr::to_string() const {
  return std::format("Kind::Arrow({}, {})", a.lhs->to_string(),
                     a.rhs->to_string());
}

bool Kind::operator==(const Kind& rhs) {
  if (this->is<KStar>() && rhs.is<KStar>()) {
    return true;
  }
  if (this->is<KStar>() || rhs.is<KStar>()) {
    return false;
  }
  return std::get<KArr>(*this).is_equal(std::get<KArr>(rhs));
}
bool Kind::operator!=(const Kind& rhs) {
  if (this->is<KStar>() && rhs.is<KStar>()) {
    return false;
  }
  if (this->is<KStar>() || rhs.is<KStar>()) {
    return true;
  }
  return std::get<KArr>(*this).is_equal(std::get<KArr>(rhs));
}

/** types */

bool TyVar::operator==(const TyVar& other) { return this->id == other.id; }
bool TyVar::operator!=(const TyVar& other) { return this->id != other.id; }
bool TyVar::operator<(const TyVar& other) { return this->id < other.id; }

std::string TyVar::to_string() {
  return "Type::Var(" + this->id.string() + ", " + this->_kind.to_string()
         + ")";
}
std::string TyCon::to_string() {
  return "Type::Con(" + this->id.string() + ", " + this->_kind.to_string()
         + ")";
}
std::string TyApp::to_string() {
  return "Type::App(" + this->lhs->to_string() + ", " + this->rhs->to_string()
         + ")";
}
std::string TyGen::to_string() {
  return "Type::Gen(" + std::to_string(this->i) + ")";
}

/// apply substitutions
Type Type::apply(Subst& s) {
  return std::visit(overloaded{[substs & ](const TyVar& tv) {
                                 auto mt = s.find(tv);
                                 if (mt == s.end()) {
                                   return tv;
                                 }
                                 return *mt;
                               },
                               [](const TyApp& ta) {
                                 return Type::App(ta.lhs->apply(subst),
                                                  ta.rhs->apply(subst));
                               },
                               [t](auto) { return t; }},
                    *this);
}

/// collect typevariables (ordered) from a Type
std::vector<TyVar> Type::tv() {
  void go(std::vector<TyVar> & vs, const Type& t) {
    std::visit(overloaded{[vs & ](const TyVar& tv) { v.push_back(tv); },
                          [vs & ](const TyApp& ta) {
                            go(vs, ta.lhs);
                            go(vs, ta.rhs);
                          },
                          [](auto) {}},
               t);
  }
  auto v = std::vecto<TyVar>();
  go(v, *this);
  return v;
}
} // namespace mangekyou
