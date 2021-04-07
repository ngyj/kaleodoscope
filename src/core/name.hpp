#include <prelude.hpp>
#include <set>
#include <string>

namespace mangekyou::name {

// @FIXME
struct FastString {
  using table_type = std::set<std::string>;
  static table_type _table;

  table_type::iterator str;

  explicit FastString(const char* str)
      : str(_table.emplace(str).first) {}
  explicit FastString(const std::string& str)
      : str(_table.emplace(str).first) {}

  std::string string() { return *(this->str); }

  bool operator==(FastString& other) { return this->str == other.str; }
  bool operator!=(FastString& other) { return this->str != other.str; }
  bool operator<(FastString& other) { return *(this->str) < *(other.str); }
  bool operator>(FastString& other) { return *(this->str) > *(other.str); }
  bool operator<=(FastString& other) { return *(this->str) <= *(other.str); }
  bool operator>=(FastString& other) { return *(this->str) >= *(other.str); }
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
