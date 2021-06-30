#pragma once
#include <prelude.hpp>
#include <functional>
#include <unordered_set>
#include <set>
#include <string>
#include <iostream>

namespace mangekyou::name {

struct FastString {
  using inner_type = std::string*;
private:
  struct less {
    constexpr bool operator()(std::string* lhs, std::string* rhs) const {
      return std::less<std::string>{}(*lhs, *rhs);
    }
  };
  struct equal_to {
    constexpr bool operator()(std::string* lhs, std::string* rhs) const {
      return std::equal_to<std::string>{}(*lhs, *rhs);
    }
    bool operator()(std::string* lhs, const char* rhs) const {
      return std::equal_to<std::string>{}(*lhs, rhs);
    }
    bool operator()(const char* lhs, std::string* rhs) const {
      return std::equal_to<std::string>{}(lhs, *rhs);
    }
  };
  struct hash {
    std::size_t operator()(std::string* x) const {
      return std::hash<std::string>{}(*x);
    }
    std::size_t operator()(const char* x) const {
      return std::hash<std::string>{}(x);
    }
  };

public:
  using table_type = std::unordered_set<std::string*, hash, equal_to>;

  static table_type s_table;

  inner_type str;

  FastString(const FastString& other)
      : str(other.str) {}
  explicit FastString(const char* str) {
    auto* x = new std::string(str);
    auto it = s_table.find(x);
    if (it == s_table.end()) {
      s_table.insert(x);
      this->str = x;
    } else {
      this->str = *it;
    }
  }
  explicit FastString(const std::string& str) {
    auto it = s_table.find((std::string*) &str);
    if (it == s_table.end()) {
      auto [it, _] = s_table.insert(new std::string(str));
      this->str = *it;
    } else {
      this->str = *it;
    }
  }

  std::string string() const { return *(this->str); }

  bool operator==(const FastString& other) const {
    return this->str == other.str;
  }
  bool operator!=(const FastString& other) const {
    return this->str != other.str;
  }
  bool operator<(const FastString& other) const {
    return *(this->str) < *(other.str);
  }
  bool operator>(const FastString& other) const {
    return *(this->str) > *(other.str);
  }
  bool operator<=(const FastString& other) const {
    return *(this->str) <= *(other.str);
  }
  bool operator>=(const FastString& other) const {
    return *(this->str) >= *(other.str);
  }
};

using Id = FastString;

/*
struct Name {
  enum class Sort { External, Internal, System } sort;
  OccName occ;
  usize uniq;

  Name(Sort sort, OccName occ, usize uniq)
      : sort(sort)
      , occ(occ)
      , uniq(uniq) {}

  static Name make_external(OccName occ, usize uniq) {
    return Name(Sort::External, occ, uniq);
  }
  static Name make_internal(OccName occ, usize uniq) {
    return Name(Sort::Internal, occ, uniq);
  }
  static Name make_system(OccName occ, usize uniq) {
    return Name(Sort::System, occ, uniq);
  }
};

struct OccName {
  enum class Sort { VarName, DataName, TvName, TcName, ClsName } sort;
  FastString name;

  OccName(Sort sort, FastString name)
      : sort(sort)
      , name(name) {}

  static OccName make_var(FastString name) {
    return OccName(Sort::VarName, name);
  }
  static OccName make_data(FastString name) {
    return OccName(Sort::DataName, name);
  }
  static OccName make_typevar(FastString name) {
    return OccName(Sort::TvName, name);
  }
  static OccName make_typecon(FastString name) {
    return OccName(Sort::TcName, name);
  }
  static OccName make_class(FastString name) {
    return OccName(Sort::ClsName, name);
  }
};
*/

} // namespace mangekyou::name
