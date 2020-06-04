#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include <iostream>
#include <sstream>

#include "syntax/parser.hpp"
#include "utils/print_visitor.hpp"

llvm::LLVMContext ctx;
std::unique_ptr<llvm::Module> module;

// @TODO instead of looping like a virgin like this, just implement a collection
// of statements parser
void loop(Parser::Parser& p) {
  using namespace Parser;
  auto printer = PrintVisitor();
  for (;;) {
    switch (p.next_token()) {
    case tok_eof:
      return;
    case ';': // ignore
      p.next_token();
      break;
    case tok_fn: {
      if (auto f = p.parse_function())
        f->accept(printer);
      else
        return;
      break;
    }
    case tok_extern: {
      if (auto e = p.parse_extern())
        e->accept(printer);
      else
        return;
      break;
    }
    default: {
      if (auto e = p.parse_expr())
        e->accept(printer);
      else
        return;
      break;
    }
    }
    std::cout << '\n' << std::flush;
  }
}

void parse_in(std::istream& src) {
  auto p = Parser::Parser(src);
  loop(p);
  std::cout << std::flush;
  fprintf(stderr, "\n--------\n");
  // module->print(llvm::errs(), nullptr);
}

void repl() {
  for (;;) {
    if (std::cin.eof()) {
      std::cout << std::endl << "Leaving." << std::endl;
      std::cout.flush();
      return;
    }
    std::cout << "> ";
    while (isspace(std::cin.peek()))
      std::cin.get();

    std::string str;
    std::getline(std::cin, str);
    auto ss = std::istringstream(str);
    parse_in(ss);
  }
}

int main() { repl(); }
