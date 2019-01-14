#include <sstream>
#include <iostream>

#include "parser.hpp"

void parse_in(std::istream& src) {
    auto p = Parser(src);
    while(p.next_token() != tok_eof)
        std::cout << p.to_string() << std::endl;
    std::cout << "end." << std::endl;
}
void repl() {
    while (1) {
        if (std::cin.eof()) {
            std::cout << std::endl << "Leaving." << std::endl;
            std::cout.flush();
            return;
        }
        std::cout << "> ";
        while(isspace(std::cin.peek()))
            std::cin.get();

        std::string str;
        std::getline(std::cin, str, ';');
        auto ss = std::istringstream(str);
        parse_in(ss);
    }
}

int main() {
    repl();
}
