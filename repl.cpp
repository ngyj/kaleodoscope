#include <iostream>
#include <sstream>

#include "repl.hpp"

#include "code/codegen.hpp"
#include "utils/print_visitor.hpp"

// @TODO instead of looping like a virgin like this, just implement a collection
// of statements parser
void Repl::loop(Parser::Parser& p) {
  using namespace Parser;
  auto printer = Print_v();
  auto gen     = Code::Codegen_v(ctx);
  for (;;) {
    switch (p.next_token()) {
    case tok_eof:
      return;
    case ';': // ignore
      p.next_token();
      break;
    case tok_fn: {
      if (auto f = p.parse_function()) {
        printf("AST:\n");
        f->accept(printer);
        f->accept(gen);
        if (auto c = gen.get_fun()) {
          printf("\nCode:\n");
          c->print(llvm::outs());
          printf("\n");
        }
      } else
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
      if (auto e = p.parse_expr()) {
        printf("AST:  ");
        e->accept(printer);
        e->accept(gen);
        if (auto c = gen.get_val()) {
          printf("\nCode:\n");
          c->print(llvm::outs());
          printf("\n");
        }

      } else
        return;
      break;
    }
    }
    std::cout << '\n' << std::flush;
  }
}

void Repl::parse_line(std::istream& src) {
  auto p = Parser::Parser(src);
  loop(p);
  std::cout << std::flush;
}

void Repl::run() {
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
    parse_line(ss);
  }
}
