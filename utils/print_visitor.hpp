#pragma once
#include <algorithm> // std::transform
#include <iostream>
#include <vector>

#include "../syntax/ast.hpp"

class PrintVisitor : public AST::Visitor {
public:
  int indent;
  int bin_depth;
  std::ostream& out;

  virtual void visit(AST::Expr* e);
  virtual void visitNumber(AST::NumberExpr* e);
  virtual void visitVariable(AST::VariableExpr* e);
  virtual void visitBinary(AST::BinaryExpr* e);
  virtual void visitCall(AST::CallExpr* e);
  virtual void visitFunction(AST::Function* f);
  virtual void visitPrototype(AST::Prototype* p);
  virtual void visitModule(AST::Module* m);
  virtual void visitStmt(AST::Stmt* s);

  void newline();
  PrintVisitor(std::ostream& o = std::cout, int start_indent = 0)
      : indent(start_indent)
      , bin_depth(0)
      , out(o) {}
};
