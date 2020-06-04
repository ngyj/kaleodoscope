#include <algorithm> // std::transform
#include <cassert>
#include <cstdlib> // abort
#include <iostream>
#include <vector>

#include "../syntax/ast.hpp"
#include "print_visitor.hpp"

void PrintVisitor::visit(AST::Expr* e) {
  std::cerr << "fuck!";
  std::abort();
}

void PrintVisitor::visitNumber(AST::NumberExpr* e) {
  assert(e);
  out << e->val();
}

void PrintVisitor::visitVariable(AST::VariableExpr* e) {
  assert(e);
  out << e->name();
}

void PrintVisitor::visitBinary(AST::BinaryExpr* e) {
  assert(e);
  out << '(' << e->op() << ' ';
  e->lhs()->accept(*this);
  out << ' ';
  e->rhs()->accept(*this);
  out << ')';
}

void PrintVisitor::visitCall(AST::CallExpr* e) {
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

void PrintVisitor::visitFunction(AST::Function* f) {
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

void PrintVisitor::visitPrototype(AST::Prototype* p) {
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

void PrintVisitor::visitModule(AST::Module* m) {
  std::cerr << "not implemented yet!!";
  std::abort();
}

void PrintVisitor::visitStmt(AST::Stmt* s) {
  std::cerr << "fuck!!";
  std::abort();
}

void PrintVisitor::newline() {
  std::cout << '\n';
  std::cout << std::string(indent, ' ');
}
