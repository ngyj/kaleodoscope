#include <algorithm> // std::transform
#include <cassert>
#include <iostream>
#include <vector>

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
  out << '(' << e->op() << ' ';
  e->lhs()->accept(*this);
  out << ' ';
  e->rhs()->accept(*this);
  out << ')';
}

void PrintVisitor::visitCall(CallExprAST * e) {
  assert(e);
  out << '(';
  out << e->callee();
  auto args = e->args();
  auto it = args.begin();
  while(it != args.end()) {
    out << ' ';
    (*it)->accept(*this);
    it++;
  }
  out << ')';
}

void PrintVisitor::visitFunction(FunctionAST* f) {
  assert(f);
  if (f->proto()->ext())
    out << "(extern";
  else
    out << "(def ";
  f->proto()->accept(*this);
  indent++;
  newline();
  f->body()->accept(*this);
  indent--;
  out << ')' << std::flush;
}

void PrintVisitor::visitPrototype(PrototypeAST * p) {
  assert(p);
  out << '(' << p->name();

  auto pars = p->args();
  auto it = pars.begin();
  while(it != pars.end()) {
    out << ' ' << *it;
    it++;
  }
  out << ')';
}

void PrintVisitor::newline() {
  std::cout << '\n';
  std::cout << std::string(indent, ' ');
}
