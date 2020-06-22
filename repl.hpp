#pragma once
#include <sstream>

#include "syntax/parser.hpp"

struct Repl {

  llvm::LLVMContext ctx;

  Repl() {}

  void run();
  void loop(Parser::Parser& p);
  void parse_line(std::istream& src);
};
