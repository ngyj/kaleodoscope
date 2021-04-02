#include <string>
#include <prelude.hpp>

// @FIXME
class SrcLoc {};
struct FastString {
    shared_ptr<std::string> str;

    FastString(const char* str) : str(make_shared<std::string>(str)){}
    FastString(std::string str) : str(make_shared<std::string>(str)){}
    FastString(std::string& str) : str(make_shared<std::string>(str)){}
    FastString(std::string&& str) : str(make_shared<std::string>(str)){}
    FastString(FastString&& str) : str(std::move(str.str)){}
    FastString(const FastString& str) : str(str.str){}

    bool operator ==(FastString& other) {
        return this->str == other.str;
    }
    bool operator !=(FastString& other) {
        return !(*this == other);
    }
    bool operator < (FastString& other) {
        return this->str < other.str;
    }
    bool operator > (FastString& other) {
        return this->str > other.str;
    }
    bool operator <= (FastString& other) {
        return this->str <= other.str;
    }
    bool operator >= (FastString& other) {
        return this->str >= other.str;
    }
};

struct Name {
    enum class Sort { External, Internal, System } sort;
    OccName occ; 
    usize uniq;

    Name(Sort sort, OccName occ, usize uniq) 
        : sort(sort), occ(occ), uniq(uniq){}

    static Name make_external(OccName occ, usize uniq) {
        return Name(Sort::External, occ, uniq);
    }
    static Name make_external(OccName occ, usize uniq) {
        return Name(Sort::Internal, occ, uniq);
    }
    static Name make_external(OccName occ, usize uniq) {
        return Name(Sort::System, occ, uniq);
    }
};

struct OccName {
    enum class Sort { VarName, DataName, TvName, TcName, ClsName } sort;
    FastString name;

    OccName(Sort sort, FastString name) : sort(sort), name(name) {}

    static OccName make_var(FastString name) { return OccName(Sort::VarName, name); } 
    static OccName make_data(FastString name) { return OccName(Sort::DataName, name); } 
    static OccName make_typevar(FastString name) { return OccName(Sort::TvName, name); } 
    static OccName make_typecon(FastString name) { return OccName(Sort::TcName, name); } 
    static OccName make_class(FastString name) { return OccName(Sort::ClsName, name); } 
};