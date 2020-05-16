#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

using llvm::Value;
using llvm::Function;

// FIXME no more globals please
extern llvm::LLVMContext ctx;
extern std::unique_ptr<llvm::Module> module;

struct Span {
  std::string filename;
  int line, begin, end;

  Span(std::string filename, int line)
    : Span(filename, line, -1, -1) {}
  Span(std::string filename, int begin, int line)
    : Span(filename, line, begin, -1) {}
  Span(std::string filename, int line, int begin, int end)
    : filename(filename), line(line), begin(begin), end(end) {}
};

class ASTVisitor;

// --- the AST ---
class ExprAST {
public:
  virtual ~ExprAST() {}
  virtual Value *codegen() = 0;

  virtual void accept(ASTVisitor& v);
};
/// number literals
class NumberExprAST : public ExprAST {
  double _val;
public:
  double val() const { return _val; }

  NumberExprAST(double val) : _val(val) {}

  virtual Value *codegen();
  virtual void accept(ASTVisitor& v);
};
/// variables
class VariableExprAST : public ExprAST {
  std::string _name;
public:
  std::string name() const { return _name; }

  VariableExprAST(const std::string &name) : _name(name) {}

  virtual Value *codegen();
  virtual void accept(ASTVisitor& v);
};
/// binary operators
class BinaryExprAST : public ExprAST {
  char _op;
  std::unique_ptr<ExprAST> _lhs, _rhs;
public:
  char op() const { return _op; }
  // not safe, shared would be better
  ExprAST* lhs() const { return _lhs.get(); }
  ExprAST* rhs() const { return _rhs.get(); }

  BinaryExprAST(char op, std::unique_ptr<ExprAST> lhs
               , std::unique_ptr<ExprAST> rhs)
    : _op(op), _lhs(std::move(lhs)), _rhs(std::move(rhs)) {}

  virtual Value *codegen();
  virtual void accept(ASTVisitor& v);
};
/// function calls
class CallExprAST : public ExprAST {
  std::string _callee;
  std::vector<std::unique_ptr<ExprAST>> _args;
public:
  std::string callee() const { return _callee; }
  std::vector<ExprAST *> args() const {
    auto r = std::vector<ExprAST *>(_args.size());
    std::transform(_args.begin(), _args.end(), r.begin(), [](auto& p){ return p.get(); });
    return r;
  }

  CallExprAST(const std::string &name
             , std::vector<std::unique_ptr<ExprAST>> args)
    : _callee(name), _args(std::move(args)) {}

  virtual Value *codegen();
  virtual void accept(ASTVisitor& v);
};
/// captures name and argument names for a function
class PrototypeAST {
  std::string _name;
  std::vector<std::string> _args;
  bool _ext;
public:

  std::string name() const { return _name; }
  std::vector<std::string> args() const { return std::vector(_args); } // copy
  bool ext() const { return _ext; }
  PrototypeAST* ext(bool ext) { _ext = ext; return this; }

  ~PrototypeAST() = default;
  PrototypeAST(const std::string &name
              , std::vector<std::string> args
              , bool ext = false)
    : _name(name), _args(std::move(args)), _ext(ext) {}

  Function *codegen();
  void accept(ASTVisitor& v);
};
/// function defintion
class FunctionAST {
  std::unique_ptr<PrototypeAST> _proto;
  std::unique_ptr<ExprAST> _body;
public:
  PrototypeAST* proto() const { return _proto.get(); }
  ExprAST* body() const { return _body.get(); }

  ~FunctionAST() = default;
  FunctionAST(std::unique_ptr<PrototypeAST> proto
             , std::unique_ptr<ExprAST> body)
    : _proto(std::move(proto)), _body(std::move(body)) {}

  Function *codegen();
  void accept(ASTVisitor& v);
};

namespace err {
  // REFACTOR move me to a better place
  void log_error(const char *str);
  Value *log_errorV(const char *str);
}

class ASTVisitor {
public:
  virtual void visit(ExprAST *) = 0;
  virtual void visitNumber(NumberExprAST *) = 0;
  virtual void visitVariable(VariableExprAST *) = 0;
  virtual void visitBinary(BinaryExprAST *) = 0;
  virtual void visitCall(CallExprAST *) = 0;
  virtual void visitPrototype(PrototypeAST *) = 0;
  virtual void visitFunction(FunctionAST *) = 0;

};
#endif
