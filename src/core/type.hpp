#include "../core/name.hpp"
#include "var.hpp"
#include <variant>
#include <vector>
#include <prelude.hpp>

// using TyVar = Var;
using KindOrType = Type;
using Id = std::string;

struct Kind;

/* Kinds */
struct Star {
    bool operator==(const Star& _) noexcept { return true; }
    bool operator!=(const Star& _) noexcept { return false; }
};
struct KArr {
    Ref<Kind> lhs;
    Ref<Kind> rhs;
    bool operator==(const KArr& other) { 
        return *(this->lhs) == *(other.lhs)
        && *(this->rhs) == *(other.rhs);
    }
    // bool operator!=(const KArr& other) { 
    //     return *(this->lhs) != *(other.lhs)
    //     || *(this->rhs) != *(other.rhs);
    // }
};
struct Kind : std::variant<Star, KArr> {
    using std::variant<Star, KArr>::variant;
};

/* Types */
template<typename T>
struct HasKind {
    Kind kind() {
        return static_cast<T*>(this)->kind();
    }
};

struct TyVar : HasKind<TyVar> {
    Id id;
    Kind _kind;
    TyVar(Id id, Kind kind) : id(id), _kind(kind) {}
};
struct TyCon {
    Id id;
    Kind kind;
};

// @FIXME
struct TCon {
    Ref<TyCon> con;
};
struct TVar {
    Ref<TyVar> var;
};
struct TApp {
    Ref<Type> fun;
    Ref<Type> arg;
};
/// generic/quantified type variables
struct TGen {
    i32 i;
};

struct TyConAppTy {
    Ref<TCon> tycon;
    std::vector<Ref<KindOrType>> args;
}; 
struct FunTy {
    Ref<Type> arg;
    Ref<Type> res;
};


Kind foo() {
    auto x = TyVar{ std::string(""), Star() };
    return x.kind();
}
// using Type = std::variant<VarTy, AppTy, TyConAppTy, FunTy, ForAllTy, LitTy>;