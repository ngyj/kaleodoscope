#pragma once
#include <iostream>
#include <vector>
#include <algorithm> // std::transform

#include "ast.hpp"

class PrintVisitor : public ASTVisitor {
public:
  int indent;
  int bin_depth;
  std::ostream& out;

  PrintVisitor(std::ostream& o = std::cout, int start_indent = 0)
    : indent(start_indent), bin_depth(0), out(o) {}

  virtual void visit(ExprAST* e);
  virtual void visitNumber(NumberExprAST * e);
  virtual void visitVariable(VariableExprAST * e);
  virtual void visitBinary(BinaryExprAST * e);
  virtual void visitCall(CallExprAST * e);
  virtual void visitFunction(FunctionAST* f);
  virtual void visitPrototype(PrototypeAST * p);
  virtual void visitModule(ModuleAST* m);
  virtual void visitStmt(StmtAST* s);

  void newline();
};
