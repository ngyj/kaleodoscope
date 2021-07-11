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
  return !(*this == rhs);
}

Kind Kind::Star() { return KStar{}; }
Kind Kind::Arrow(const Rc<Kind>& lhs, const Rc<Kind>& rhs) {
  return KArr{ lhs, rhs };
}
Kind Kind::mkUnary() {
  return Kind::Arrow(make_shared<Kind>(Kind::Star()), make_shared<Kind>(Kind::Star()));
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

auto Type::Unit = Type::Gen(0);
auto Type::Char = Type::Con(name::FastString("Char"), Kind::Star());
auto Type::Int = Type::Con(name::FastString("Int"), Kind::Star());
auto Type::Integer = Type::Con(name::FastString("Integer"), Kind::Star());
auto Type::Float = Type::Con(name::FastString("Float"), Kind::Star());
auto Type::Double = Type::Con(name::FastString("Double"), Kind::Star());

// TODO
// auto Type::List = Type::Con(FastString("[]"), Kind::Arrow());
// auto Type::Arrow = Type::Con(FastString("->"), Kind::Arrow());
// auto Type::Tuple2 = Type::Con(FastString("(,)"), Kind::Arrow());
} // namespace mangekyou
