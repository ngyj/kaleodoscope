#pragma once
#include <memory>
#include <string>
#include <vector>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

namespace AST {
using std::shared_ptr;
using std::unique_ptr;

struct Span {
  std::string filename;
  int line, endl, begin, end;

  Span(std::string filename)
      : Span(filename, 1, 0, -1) {}
  Span(std::string filename, int line)
      : Span(filename, line, 0, -1) {}
  Span(std::string filename, int line, int begin)
      : Span(filename, line, begin, -1) {}
  Span(std::string filename, int line, int begin, int end)
      : filename(filename)
      , line(line)
      , begin(begin)
      , end(end) {}
  Span(std::string filename, int line, int endl, int begin, int end)
      : filename(filename)
      , line(line)
      , endl(line)
      , begin(begin)
      , end(end) {}

  int newline() {
    begin = 0;
    return ++line;
  }
  int next() { return ++begin; }
  /// Helper to make a span from the argument and to the begin.
  /// Since getc() and next() used in it increments begin
  /// when we reach the end of the lexeme begin points to its end.
  Span mkfrom(int start) const {
    return Span(filename, line, start, begin - start);
  }
  Span rangeFrom(Span& span) const {
    return Span(filename, span.line, endl, span.begin, end);
  }
  Span rangeTo(Span& span) const {
    return Span(filename, line, span.endl, begin, span.end);
  }
};

struct Visitor;

// --- the AST ---
class Expr {
public:
  Span span;
  virtual ~Expr() {}
  Expr(const Span& span)
      : span(span) {}
  virtual llvm::Value* codegen() = 0;

  virtual void accept(Visitor& v);
};
/// number literals
class NumberExpr : public Expr {
  double _val;

public:
  double val() const { return _val; }

  NumberExpr(double val, const Span& span)
      : Expr(span)
      , _val(val) {}

  virtual llvm::Value* codegen();
  virtual void accept(Visitor& v);
};
/// variables
class VariableExpr : public Expr {
  std::string _name;

public:
  std::string name() const { return _name; }

  VariableExpr(const std::string& name, const Span& span)
      : Expr(span)
      , _name(name) {}

  virtual llvm::Value* codegen();
  virtual void accept(Visitor& v);
};
/// binary operators
class BinaryExpr : public Expr {
  char _op;
  std::unique_ptr<Expr> _lhs, _rhs;

public:
  char op() const { return _op; }
  // not safe, shared would be better
  Expr* lhs() const { return _lhs.get(); }
  Expr* rhs() const { return _rhs.get(); }

  BinaryExpr(char op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs,
             const Span& span)
      : Expr(span)
      , _op(op)
      , _lhs(std::move(lhs))
      , _rhs(std::move(rhs)) {}

  virtual llvm::Value* codegen();
  virtual void accept(Visitor& v);
};
/// function calls
class CallExpr : public Expr {
  std::string _callee;
  std::vector<std::unique_ptr<Expr>> _args;

public:
  std::string callee() const { return _callee; }
  std::vector<Expr*> args() const {
    auto r = std::vector<Expr*>(_args.size());
    std::transform(_args.begin(), _args.end(), r.begin(),
                   [](auto& p) { return p.get(); });
    return r;
  }

  CallExpr(const std::string& name, std::vector<std::unique_ptr<Expr>> args,
           const Span& span)
      : Expr(span)
      , _callee(name)
      , _args(std::move(args)) {}

  virtual llvm::Value* codegen();
  virtual void accept(Visitor& v);
};

/// captures name and argument names for a function
class Prototype {
  std::string _name;
  std::vector<std::string> _args;
  bool _ext;

public:
  std::string name() const { return _name; }
  std::vector<std::string> args() const { return std::vector(_args); } // copy
  bool ext() const { return _ext; }
  Prototype* ext(bool ext) {
    _ext = ext;
    return this;
  }

  ~Prototype() = default;
  Prototype(const std::string& name, std::vector<std::string> args,
            bool ext = false)
      : _name(name)
      , _args(std::move(args))
      , _ext(ext) {}

  llvm::Function* codegen();
  void accept(Visitor& v);
};

class Stmt {
public:
  virtual ~Stmt()                 = default;
  virtual void accept(Visitor& v) = 0;
};

/// function defintion
class Function : Stmt {
  std::unique_ptr<Prototype> _proto;
  std::unique_ptr<Expr> _body;

public:
  Prototype* proto() const { return _proto.get(); }
  Expr* body() const { return _body.get(); }

  ~Function() = default;
  Function(std::unique_ptr<Prototype> proto, std::unique_ptr<Expr> body)
      : _proto(std::move(proto))
      , _body(std::move(body)) {}

  llvm::Function* codegen();
  void accept(Visitor& v);
};

class Module {
  std::string _name;
  std::vector<shared_ptr<Stmt>> _stmts;

public:
  std::vector<shared_ptr<Stmt>> stmts() const { return _stmts; }
  std::string name() const { return _name; }

  ~Module() = default;
  Module(std::string& name, std::vector<shared_ptr<Stmt>>& stmts)
      : _name(name)
      , _stmts(stmts) {}
  void accept(Visitor& v);
};

struct Visitor {
  virtual void visit(Expr*)                 = 0;
  virtual void visitNumber(NumberExpr*)     = 0;
  virtual void visitVariable(VariableExpr*) = 0;
  virtual void visitBinary(BinaryExpr*)     = 0;
  virtual void visitCall(CallExpr*)         = 0;
  virtual void visitPrototype(Prototype*)   = 0;
  virtual void visitFunction(Function*)     = 0;
  virtual void visitModule(Module*)         = 0;
  virtual void visitStmt(Stmt*)             = 0;
};
} // namespace AST

namespace err {
// REFACTOR move me to a better place
void log_error(const char* str);
llvm::Value* log_errorV(const char* str);
} // namespace err
