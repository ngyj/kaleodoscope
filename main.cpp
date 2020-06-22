#include <iostream>

#include "repl.hpp"

int main(int argc, char** argv) {
  if (argc == 1)
    Repl().run();
  else
    std::cerr << "no args supported\n" << std::flush;
}
