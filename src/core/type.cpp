#include "type.hpp"
#include <variant>

namespace mangekyou {
bool KArr::is_equal(const KArr& rhs) const {
  return *(this->lhs) == *(rhs.lhs) && *(this->rhs) == *(rhs.rhs);
}
std::string KStar::to_string() const { return "Kind::Star"; }
std::string KArr::to_string() const {
  return "Kind::Arrow(" + this->lhs->to_string() + ", " + this->rhs->to_string()
         + ")";
}

bool Kind::operator==(const Kind& rhs) const {
  if (this->is<KStar>() && rhs.is<KStar>()) {
    return true;
  }
  if (this->is<KStar>() || rhs.is<KStar>()) {
    return false;
  }
  return std::get<KArr>(*this).is_equal(std::get<KArr>(rhs));
}
bool Kind::operator!=(const Kind& rhs) const {
  if (this->is<KStar>() && rhs.is<KStar>()) {
    return false;
  }
  if (this->is<KStar>() || rhs.is<KStar>()) {
    return true;
  }
  return std::get<KArr>(*this).is_equal(std::get<KArr>(rhs));
}

/** types */

bool TyVar::operator==(const TyVar& other) const {
  return this->id == other.id;
}
bool TyVar::operator!=(const TyVar& other) const {
  return this->id != other.id;
}
bool TyVar::operator<(const TyVar& other) const { return this->id < other.id; }

std::string TyVar::to_string() const {
  return "Type::Var(" + this->id.string() + ", " + this->_kind.to_string()
         + ")";
}
std::string TyCon::to_string() const {
  return "Type::Con(" + this->id.string() + ", " + this->_kind.to_string()
         + ")";
}
std::string TyApp::to_string() const {
  return "Type::App(" + this->lhs->to_string() + ", " + this->rhs->to_string()
         + ")";
}
std::string TyGen::to_string() const {
  return "Type::Gen(" + std::to_string(this->i) + ")";
}

/// apply substitutions
// Rc<Type> Type::apply(Subst& s) {
//   return std::visit(
//       overloaded{[s](const TyVar& tv) -> Rc<Type> {
//                    auto mt = s.find(tv);
//                    if (mt == s.end()) {
//                      return make_shared<Type>(tv);
//                    }
//                    return make_shared<Type>(*mt);
//                  },
//                  [s](const TyApp& ta) -> Rc<Type> {
//                    return Type::App(ta.lhs->apply(s), ta.rhs->apply(s));
//                  },
//                  [this](auto) -> Rc<Type> { return make_shared<Type>(*this);
//                  }},
//       *this);
// }

void tv_go(std::vector<TyVar>& vs, const Type& t) {
  std::visit(overloaded{[&vs](const TyVar& tv) { vs.push_back(tv); },
                        [&vs](const TyApp& ta) {
                          tv_go(vs, *ta.lhs);
                          tv_go(vs, *ta.rhs);
                        },
                        [](auto) {}},
             t);
}
/// collect typevariables (ordered) from a Type
std::vector<TyVar> Type::tv() {
  auto v = std::vector<TyVar>();
  tv_go(v, *this);
  return v;
}
} // namespace mangekyou
