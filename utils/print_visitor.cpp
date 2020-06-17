#include <algorithm> // std::transform
#include <cassert>
#include <cstdlib> // abort
#include <iostream>
#include <vector>

#include "../syntax/ast.hpp"
#include "print_visitor.hpp"

void Print_v::visit(AST::Expr* e) {
  std::cerr << "fuck!";
  std::abort();
}

void Print_v::visitNumber(AST::NumberExpr* e) {
  assert(e);
  out << e->val();
}

void Print_v::visitVariable(AST::VariableExpr* e) {
  assert(e);
  out << e->name();
}

void Print_v::visitBinary(AST::BinaryExpr* e) {
  assert(e);
  out << '(' << e->op() << ' ';
  e->lhs()->accept(*this);
  out << ' ';
  e->rhs()->accept(*this);
  out << ')';
}

void Print_v::visitCall(AST::CallExpr* e) {
  assert(e);
  out << '(';
  out << e->callee();
  auto args = e->args();
  auto it   = args.begin();
  while (it != args.end()) {
    out << ' ';
    (*it)->accept(*this);
    it++;
  }
  out << ')';
}

void Print_v::visitFunction(AST::Function* f) {
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

void Print_v::visitPrototype(AST::Prototype* p) {
  assert(p);
  out << '(' << p->name();

  auto pars = p->args();
  auto it   = pars.begin();
  while (it != pars.end()) {
    out << ' ' << *it;
    it++;
  }
  out << ')';
}

void Print_v::visitModule(AST::Module* m) {
  std::cerr << "not implemented yet!!";
  std::abort();
}

void Print_v::visitStmt(AST::Stmt* s) {
  std::cerr << "fuck!!";
  std::abort();
}

void Print_v::newline() {
  std::cout << '\n';
  std::cout << std::string(indent, ' ');
}
