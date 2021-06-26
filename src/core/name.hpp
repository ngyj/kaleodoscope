#pragma once
#include <prelude.hpp>
#include <map>
#include <string>
#include <iostream>

namespace mangekyou::name {

// @FIXME
struct FastString {
  using table_type = std::map<std::string, Rc<std::string>>;
  static table_type s_table;

  Rc<std::string> str;

  FastString(const FastString& other)
      : str(other.str) {}
  explicit FastString(const char* str) {
    auto& ref = s_table[str];
    if (!ref)
      ref = make_shared<std::string>(str);
    this->str = ref;
  }
  explicit FastString(const std::string& str) {
    auto& ref = s_table[str];
    if (!ref)
      ref = make_shared<std::string>(str);
    this->str = ref;
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
/**/

} // namespace mangekyou::name
