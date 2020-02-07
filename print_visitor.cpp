#include <iostream>
#include <vector>
#include <algorithm> // std::transform
#include <cassert>

#include "ast.hpp"
#include "print_visitor.hpp"


void PrintVisitor::visit(ExprAST* e) {
  std::cout << "fuck!";
  return;
}

void PrintVisitor::visitNumber(NumberExprAST * e) {
  assert(e);
  out << e->val();
}

void PrintVisitor::visitVariable(VariableExprAST * e) {
  assert(e);
  out << e->name();
}

void PrintVisitor::visitBinary(BinaryExprAST * e) {
  assert(e);
  if (bin_depth++)
    out << '(';
  e->lhs()->accept(*this);
  out << ' ' << e->op() << ' ';
  e->rhs()->accept(*this);
  if (--bin_depth)
    out << ')';
}

void PrintVisitor::visitCall(CallExprAST * e) {
  assert(e);
  out << e->callee();
  out << '(';
  int i = 0;
  for(auto it : e->args()) {
    if (i++)
      out << ", ";
    it->accept(*this);
  }
  out << ')';
}

void PrintVisitor::visitFunction(FunctionAST* f) {
  assert(f);
  out << "def ";
  f->proto()->accept(*this);
  indent++;
  newline();
  f->body()->accept(*this);
  indent--;
}

void PrintVisitor::visitPrototype(PrototypeAST * p) {
  assert(p);
  if (p->ext())
    out << "extern ";
  out << p->name();

  out << '(';
  int i = 0;
  for(auto& arg : p->args()) {
    if (i++)
      out << ", ";
    out << arg;
  }
  out << ')';
  if (p->ext())
    out << ";";
}

void PrintVisitor::newline() {
  std::cout << '\n';
  std::cout << std::string(' ', indent);
}
